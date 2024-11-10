#include "fmt/base.h"
#include <bitmap_index/bitmap_index.hpp>
#include <bitmap_index/bitmap_merge.hpp>
#include <bitmap_index/helpers.hpp>

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

namespace bitmap_index {

BitmapIndex::BitmapIndex(fs::path dir)
    : lsmTree_(
        dir,
        lsm::DEFAULT_MAX_MEMTABLE_SIZE,
        lsm::DEFAULT_SPARSE_INDEX_FREQUENCY,
        bitmap_index::bitmapMerge)
{ }

BitmapIndex::~BitmapIndex()
{
    assert(finished_);
}

void BitmapIndex::finish()
{
    dump();
    lsmTree_.finish();
    finished_ = true;
}

void BitmapIndex::insert(const std::string& key, uint64_t value)
{
    memtable_[key].add(value);
    if (memtable_.size() >= lsm::DEFAULT_MAX_MEMTABLE_SIZE) {
        dump();
    }
}

roaring::Roaring64Map BitmapIndex::getByKey(const std::string& key)
{
    roaring::Roaring64Map bitmap;
    if (memtable_.contains(key)) {
        bitmap |= memtable_[key];
    }
    for (auto& row : lsmTree_.getByKeyRange(key, key)) {
        bitmap |= readBitmap(row.value());
    }
    return bitmap;
}

std::generator<roaring::Roaring64Map&> BitmapIndex::getByKeyRange(std::string left, std::string right)
{
    auto memtableIter = memtable_.lower_bound(left);
    roaring::Roaring64Map bitmap;
    for (auto& row : lsmTree_.getByKeyRange(left, right)) {
        while (memtableIter != memtable_.end() && memtableIter->first < row.key()) {
            bitmap = memtableIter->second;
            co_yield bitmap;
            ++memtableIter;
        }
        bitmap = readBitmap(row.value());
        if (memtableIter != memtable_.end() && memtableIter->first == row.key()) {
            bitmap |= memtableIter->second;
            ++memtableIter;
        }
        co_yield bitmap;
    }

    while (memtableIter != memtable_.end() && memtableIter->first <= right) {
        bitmap = memtableIter->second;
        co_yield bitmap;
        ++memtableIter;
    }
}

void BitmapIndex::dump()
{
    for (auto& [k, v] : memtable_) {
        lsmTree_.insert(lsm::structures::Row(std::move(k), writeBitmap(v)));
    }
    memtable_.clear();
    lsmTree_.dump();
}

} // namespace bitmap_index
