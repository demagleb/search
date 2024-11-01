#include <bitmap_index/bitmap_merge.hpp>
#include <bitmap_index/helpers.hpp>

#include <set>

namespace bitmap_index {

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

std::generator<lsm::structures::Row&> bitmapMerge(std::vector<std::generator<lsm::structures::Row&>> ranges, bool removeTombstones)
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
    roaring::Roaring64Map bitmap;

    while (!queue.empty()) {
        auto index = queue.begin()->index;
        queue.erase(queue.begin());

        auto& iterator = iterators[index];
        auto& row = *iterator;

        if (row.key() != lastKey) {
            if (!lastKey.empty()) {
                lsm::structures::Row row(std::move(lastKey), writeBitmap(bitmap));
                co_yield row;
                bitmap.clear();
            }
            lastKey = row.key();
        }
        bitmap |= readBitmap(row.value());

        ++iterator;
        if (iterator != ranges[index].end()) {
            queue.insert(IndexWithCompare{.iterators = iterators, .index = index});
        }
    }
    if (!lastKey.empty()) {
        lsm::structures::Row row(std::move(lastKey), writeBitmap(bitmap));
        co_yield row;
    }
}

} // namespace bitmap_index
