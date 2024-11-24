#include "fmt/base.h"
#include <positional_index/positional_merge.hpp>
#include <positional_index/structures/postings_list.hpp>
#include <positional_index/helpers.hpp>

#include <set>

namespace positional_index {

namespace {

struct IndexWithCompare {
    std::vector<std::generator<lsm::structures::Row&>::iterator>& iterators;
    size_t index;

    bool operator<(auto other) const {
        return std::tie((*iterators[index]).key(), index) <
            std::tie((*other.iterators[other.index]).key(), other.index);
    }
};

} // namespace

std::generator<lsm::structures::Row&> positionalMerge(std::vector<std::generator<lsm::structures::Row&>> ranges, bool removeTombstones)
{
    std::vector<std::generator<lsm::structures::Row&>::iterator> iterators;

    std::set<IndexWithCompare> queue;
    for (size_t index = 0; index < ranges.size(); ++index) {
        iterators.push_back(ranges[index].begin());
        if (iterators[index] != ranges[index].end()) {
            queue.insert(IndexWithCompare{.iterators = iterators, .index = index});
        }
    }
    std::string lastKey;
    std::optional<std::generator<structures::Posting&>> postings;

    while (!queue.empty()) {
        auto index = queue.begin()->index;
        queue.erase(queue.begin());

        auto& iterator = iterators[index];
        auto& row = *iterator;

        if (row.key() != lastKey) {
            if (!lastKey.empty() && postings) {
                lsm::structures::Row row(std::move(lastKey), structures::writePostingsList(std::move(*postings)));
                postings = std::nullopt;
                co_yield row;
            }
            lastKey = row.key();
        }
        if (postings) {
            postings = structures::mergePostingsLists(std::move(*postings), structures::readPostingsList(std::move(row.value())));
        } else {
            postings = structures::readPostingsList(std::move(row.value()));
        }

        ++iterator;
        if (iterator != ranges[index].end()) {
            queue.insert(IndexWithCompare{.iterators = iterators, .index = index});
        }
    }
    if (!lastKey.empty()) {
        lsm::structures::Row row(std::move(lastKey), structures::writePostingsList(std::move(*postings)));
        postings = std::nullopt;
        co_yield row;
    }
}

} // namespace positional_index
