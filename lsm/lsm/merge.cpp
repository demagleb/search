#include <lsm/merge.hpp>

#include <set>

namespace lsm {

namespace {

struct IteratorWithIndex {
    IteratorWithIndex(IteratorWithIndex&) = delete;
    IteratorWithIndex& operator=(IteratorWithIndex&) = delete;

    IteratorWithIndex(IteratorWithIndex&&) = default;
    IteratorWithIndex& operator=(IteratorWithIndex&&) = default;

    IteratorWithIndex(size_t index, std::generator<structures::Row&>::iterator iterator)
        : index(index)
        , iterator(std::move(iterator))
    { }

    size_t index;
    std::generator<structures::Row&>::iterator iterator;

    bool operator<(const IteratorWithIndex& other) {
        return std::tie((*iterator).key(), index) <
            std::tie((*other.iterator).key(), other.index);
    }
};

} // namespace


std::generator<structures::Row&> merge(std::vector<std::generator<structures::Row&>> ranges)
{
    std::set<IteratorWithIndex> queue;
    for (size_t index = 0; index < ranges.size(); ++index) {
        queue.insert(IteratorWithIndex(index, ranges[index].begin()));
    }

    std::string lastKey;
    while (!queue.empty()) {
        auto iteratorWithIndex = std::move(*queue.begin());
        auto row = std::move(*queue.begin().iterator);
        ++iteratorWithIndex.iterator;
        queue.insert(std::move(iteratorWithIndex));
        bool dup = row.key() == lastKey;

        if (row.key() == lastKey) {
            continue;
        }
        co_yield row;
    }
}

} // namespace lsm
