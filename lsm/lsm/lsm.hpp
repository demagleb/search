#pragma once

#include <lsm/structures/indexed_sstable.hpp>
#include <lsm/structures/memtable.hpp>
#include <lsm/structures/row.hpp>

#include <filesystem>
#include <generator>
#include <optional>
#include <vector>

namespace lsm {

namespace {

namespace fs = std::filesystem;

constexpr size_t DEFAULT_MAX_MEMTABLE_SIZE = 1e6;
constexpr size_t DEFAULT_SPARSE_INDEX_FREQUENCY = 100;

} // namespace

class LSMTree {
public:
    LSMTree(
        fs::path lsmTreeDir,
        size_t maxMemtableSize = DEFAULT_MAX_MEMTABLE_SIZE,
        size_t sparseIndexFrequency = DEFAULT_SPARSE_INDEX_FREQUENCY);

    std::optional<structures::Row> getByKey(const std::string& key);
    std::generator<structures::Row&> getByKeyRange(const std::string& left, const std::string& right);

    void insert(std::generator<structures::Row&> rows);
    void dump();

    void finalize() &&;

private:
    void dump(std::generator<structures::Row&> values, size_t expectedKeysCount, uint32_t level);

    const fs::path lsmTreeDir_;
    const size_t maxMemtableSize_;
    const size_t sparseIndexFrequency_;

    structures::MemTable memtable_;
    std::map<uint32_t, structures::IndexedSSTable> tables_;
};

} // namespace lsm
