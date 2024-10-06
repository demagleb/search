#pragma once

#include <lsm/structures/bloomfilter.hpp>
#include <lsm/structures/row.hpp>
#include <lsm/structures/sparse_index.hpp>
#include <lsm/structures/sstable.hpp>

#include <fstream>
#include <generator>
#include <optional>
#include <filesystem>

namespace lsm::structures {

namespace {

namespace fs = std::filesystem;

} // namespace

class IndexedSSTable {
public:
    IndexedSSTable(
        const fs::path& sstablePath,
        const fs::path& bloomFilterPath,
        const fs::path& indexPath);

    IndexedSSTable(IndexedSSTable&&) = default;

    std::optional<structures::Row> getByKey(const std::string& key);
    std::generator<Row&> getByKeyRange(const std::string& left, const std::string& right);
    std::generator<Row&> getAll();

private:
    std::ifstream sstableStream_;
    SSTable sstable_;
    structures::BloomFilter bloomFilter_;
    structures::SparseIndex index_;
};


} // namespace lsm::structures
