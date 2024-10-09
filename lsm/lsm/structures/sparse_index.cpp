#include <lsm/structures/sparse_index.hpp>
#include <lsm/structures/sstable.hpp>

namespace lsm::structures {

std::streamoff SparseIndex::getStartPos(const std::string& key) const
{
    auto iter = index_.upper_bound(key);

    if (iter == index_.begin()) {
        return 0;
    }
    --iter;
    return iter->second;
}

void SparseIndex::push_back(std::string key, std::streamoff value)
{
    index_.emplace_hint(index_.end(), std::move(key), value);
}

namespace marshal::impl {

template <>
SparseIndex fromStreamImpl<SparseIndex>(std::istream& istream)
{
    SSTable sstable(istream);
    SparseIndex::TMap index;
    for (Row& row : sstable.getAll()) {
        std::streamoff value = std::stol(row.value());
        index.emplace_hint(index.end(), std::move(row.key()), value);
    }
    return SparseIndex(std::move(index));
}

template <>
std::streamoff toStreamImpl<SparseIndex>(const SparseIndex& value, std::ostream& ostream)
{
    auto pos = ostream.tellp();
    auto rows = [&] -> std::generator<Row&> {
        for (const auto& [k, v] : value.index()) {
            Row row(k, std::to_string(v));
            co_yield row;
        }
    };

    writeSSTable(ostream, rows());
    return pos;
}

} // namespace marshal::impl

} // namespace lsm::structures
