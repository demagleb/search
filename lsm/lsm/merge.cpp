#include <lsm/merge.hpp>

#include <set>

namespace lsm {

namespace {

struct IndexWithCompare {
    std::vector<std::generator<structures::Row&>::iterator>& iterators;
    size_t index;

    bool operator<(auto other) const {
        return std::tie((*iterators[index]).key(), index) <
            std::tie((*other.iterators[other.index]).key(), other.index);
    }
};

} // namespace

std::generator<structures::Row&> merge(std::vector<std::generator<structures::Row&>> ranges, bool removeTombstones)
{
    std::vector<std::generator<structures::Row&>::iterator> iterators;

    std::set<IndexWithCompare> queue;
    for (size_t index = 0; index < ranges.size(); ++index) {
        iterators.push_back(ranges[index].begin());
        if (iterators[index] != ranges[index].end()) {
            queue.insert(IndexWithCompare{.iterators = iterators, .index = index});
        }
    }

    std::string lastKey;
    while (!queue.empty()) {
        auto index = queue.begin()->index;
        queue.erase(queue.begin());

        auto& iterator = iterators[index];
        auto& row = *iterator;
        if (row.key() != lastKey) {
            lastKey = row.key();
            if (!removeTombstones || !row.value().empty()) {
                co_yield row;
            }
        }
        ++iterator;

        if (iterator != ranges[index].end()) {
            queue.insert(IndexWithCompare{.iterators = iterators, .index = index});
        }
    }
}

} // namespace lsm
