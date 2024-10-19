#include <inverted_index/inverted_index.hpp>
#include <inverted_index/tokenize/tokenize.hpp>
#include <inverted_index/helpers.hpp>
#include <inverted_index/bitmap_merge.hpp>

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

namespace inverted_index {

InvertedIndex::InvertedIndex(fs::path dir)
    : lsmTree_(
        dir,
        lsm::DEFAULT_MAX_MEMTABLE_SIZE,
        lsm::DEFAULT_SPARSE_INDEX_FREQUENCY,
        inverted_index::bitmapMerge)
{ }

InvertedIndex::~InvertedIndex()
{
    assert(finished_);
}

void InvertedIndex::finish()
{
    dump();
    lsmTree_.finish();
    finished_ = true;
}

void InvertedIndex::insert(uint64_t key, const std::string& doc)
{
    auto words = tokenize::tokenize(doc);
    for (auto& word : words) {
        memtable_[word].add(key);
    }
    if (memtable_.size() >= lsm::DEFAULT_MAX_MEMTABLE_SIZE) {
        dump();
    }
}

roaring::Roaring64Map InvertedIndex::getByWord(std::string word)
{
    auto preparedWord = tokenize::prepareWord(word);
    if (!preparedWord) {
        return {};
    }
    word = std::move(*preparedWord);
    roaring::Roaring64Map bitmap;
    if (memtable_.contains(word)) {
        bitmap |= memtable_[word];
    }
    std::vector<lsm::Row> rows;
    for (auto& row : lsmTree_.getByKeyRange(word, word)) {
        rows.push_back(std::move(row));
    }
    assert(rows.size() <= 1);
    if (rows.size() != 0) {
        bitmap |= readBitmap(rows.front().value());
    }
    return bitmap;
}

void InvertedIndex::dump()
{
    for (auto& [k, v] : memtable_) {
        lsmTree_.insert(lsm::structures::Row(std::move(k), writeBitmap(v)));
    }
    memtable_.clear();
    lsmTree_.dump();
}

} // namespace inverted_index
