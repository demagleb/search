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

roaring::Roaring64Map BitmapIndex::get(const std::string& key)
{
    roaring::Roaring64Map bitmap;
    if (memtable_.contains(key)) {
        bitmap |= memtable_[key];
    }
    std::vector<lsm::Row> rows;
    for (auto& row : lsmTree_.getByKeyRange(key, key)) {
        rows.push_back(std::move(row));
    }
    assert(rows.size() <= 1);
    if (rows.size() != 0) {
        bitmap |= readBitmap(rows.front().value());
    }
    return bitmap;
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
