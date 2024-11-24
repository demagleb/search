#include "__generator.hpp"
#include "fmt/base.h"
#include "positional_index/structures/postings_list.hpp"
#include <cstdint>
#include <positional_index/positional_index.hpp>
#include <positional_index/positional_merge.hpp>
#include <positional_index/helpers.hpp>

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

#include <fmt/format.h>

namespace positional_index {

PositionalIndex::PositionalIndex(fs::path dir)
    : lsmTree_(
        dir,
        lsm::DEFAULT_MAX_MEMTABLE_SIZE,
        lsm::DEFAULT_SPARSE_INDEX_FREQUENCY,
        positional_index::positionalMerge)
{ }

PositionalIndex::~PositionalIndex()
{
    assert(finished_);
}

void PositionalIndex::finish()
{
    dump();
    lsmTree_.finish();
    finished_ = true;
}

void PositionalIndex::insert(const std::string& key, uint64_t value, uint64_t position)
{
    auto& row = memtable_[key];
    auto iter = row.find(structures::Posting(position, {}));
    structures::Posting posting(position, {});
    if (iter != row.end()) {
        posting = std::move(*iter);
        row.erase(*iter);
    }
    posting.bitmap().add(value);
    row.insert(std::move(posting));
    if (memtable_.size() >= lsm::DEFAULT_MAX_MEMTABLE_SIZE) {
        dump();
    }
}

std::generator<structures::Posting&> PositionalIndex::getByKey(std::string key)
{
    std::optional<std::generator<structures::Posting&>> result;
    for (auto& row : lsmTree_.getByKeyRange(key, key)) {
        result = structures::readPostingsList(std::move(row.value()));
    }
    if (memtable_.contains(key)) {
        if (!result) {
            return toGenerator(memtable_[key]);
        }
        return structures::mergePostingsLists(toGenerator(memtable_[key]), std::move(*result));
    }
    return std::move(*result);
}

std::generator<std::generator<structures::Posting&>&> PositionalIndex::getByKeyRange(std::string left, std::string right)
{
    auto memtableIter = memtable_.lower_bound(left);
    for (auto& row : lsmTree_.getByKeyRange(left, right)) {
        while (memtableIter != memtable_.end() && memtableIter->first < row.key()) {
            auto postings = toGenerator(memtableIter->second);
            co_yield postings;
            ++memtableIter;
        }
        auto posting = structures::readPostingsList(row.value());
        if (memtableIter != memtable_.end() && memtableIter->first == row.key()) {
            posting = structures::mergePostingsLists(std::move(posting), toGenerator(memtableIter->second));
            ++memtableIter;
        }
        co_yield posting;
    }

    while (memtableIter != memtable_.end() && memtableIter->first <= right) {
        auto postings = toGenerator(memtableIter->second);
        co_yield postings;
        ++memtableIter;
    }
}

namespace {

roaring::Roaring64Map getWindowsIntersection(std::span<std::deque<structures::Posting>> windows, size_t position = 0) {
    if (windows.empty()) {
        return {};
    }
    const auto& postings = windows.front();
    roaring::Roaring64Map result;
    for (auto& posting : postings) {
        if (posting.position() < position) {
            continue;
        }
        if (windows.size() == 1) {
            result |= posting.bitmap();
        } else {
            result |= posting.bitmap() & getWindowsIntersection(windows.subspan(1), posting.position() + 1);
        }
    }
    return result;
}

} // namespace

roaring::Roaring64Map PositionalIndex::getWithSequence(const std::vector<std::string> &sequence, uint8_t tolerance)
{

    std::vector<std::generator<structures::Posting&>> postingsLists;
    std::vector<std::generator<structures::Posting&>::iterator> iterators;
    std::vector<std::deque<structures::Posting>> windows;
    for (const auto& elem : sequence) {
        postingsLists.push_back(getByKey(elem));
        iterators.push_back(postingsLists.back().begin());
        windows.push_back({});
    }
    roaring::Roaring64Map result;
    while (iterators[0] != postingsLists[0].end()) {
        windows[0].push_back(std::move(*iterators[0]));
        ++iterators[0];
        size_t position = windows[0].front().position();
        for (size_t i = 1; i < postingsLists.size(); ++i) {
            while (iterators[i] != postingsLists[i].end() && (*iterators[i]).position() <= position + tolerance + sequence.size()) {
                windows[i].push_back(std::move(*iterators[i]));
                ++iterators[i];
            }
            while (!windows[i].empty() && windows[i].front().position() <= position) {
                windows[i].pop_front();
            }
        }
        result |= getWindowsIntersection(windows);
        windows[0].pop_front();
    }
    for (size_t i = 0; i < postingsLists.size(); ++i) {
        while (iterators[i] != postingsLists[i].end()) {
            ++iterators[i];
        }
    }
    return result;
}

void PositionalIndex::dump()
{
    for (auto& [k, v] : memtable_) {
        lsmTree_.insert(lsm::structures::Row(std::move(k), structures::writePostingsList(toGenerator(v))));
    }
    memtable_.clear();
    lsmTree_.dump();
}

} // namespace positional_index
