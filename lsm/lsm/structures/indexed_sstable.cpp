#include <fstream>
#include <lsm/structures/indexed_sstable.hpp>

namespace lsm::structures {

IndexedSSTable::IndexedSSTable(
    const fs::path& sstablePath,
    const fs::path& bloomFilterPath,
    const fs::path& indexPath)
    : sstableStream_(std::make_unique<std::ifstream>(sstablePath, std::ios_base::binary))
    , sstable_(*sstableStream_)
    , bloomFilter_([&] {
        std::ifstream stream(bloomFilterPath, std::ios_base::binary);
        return marshal::fromStream<structures::BloomFilter>(stream);
    }())
    , index_([&] {
        std::ifstream stream(indexPath, std::ios_base::binary);
        return marshal::fromStream<structures::SparseIndex>(stream);
    }())
{
}

std::optional<Row> IndexedSSTable::getByKey(const std::string& key)
{
    if (!bloomFilter_.hasValueFalsePositive(key)) {
        return std::nullopt;
    }
    auto offset = index_.getStartPos(key);
    sstable_.setPos(offset);

    Row row;
    while (!sstable_.end()) {
        row = sstable_.getRow();
        if (row.key() == key) {
            return row;
        }
        if (row.key() > key) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::generator<Row&> IndexedSSTable::getByKeyRange(const std::string& left, const std::string& right)
{
    auto offset = index_.getStartPos(left);
    sstable_.setPos(offset);

    Row row;
    while (!sstable_.end()) {
        row = sstable_.getRow();
        if (row.key() >= left) {
            co_yield row;
            break;
        }
    }

    while (!sstable_.end()) {
        row = sstable_.getRow();
        if (row.key() > right) {
            break;
        }
        co_yield row;
    }
}

std::generator<Row&> IndexedSSTable::getAll()
{
    sstable_.setPos(0);
    return sstable_.getAll();
}

} // namespace lsm::structures
