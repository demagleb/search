#include "lsm/structures/memtable.hpp"
#include "lsm/structures/row.hpp"
#include <cstdint>
#include <filesystem>
#include <lsm/lsm.hpp>
#include <lsm/merge.hpp>
#include <lsm/structures/bloomfilter.hpp>
#include <lsm/structures/indexed_sstable.hpp>
#include <lsm/structures/sstable.hpp>

#include <fmt/format.h>

#include <set>

namespace lsm {

namespace {

// Should be in meta

fs::path SSTablePath(fs::path dir, uint32_t level)
{
    dir /= fmt::format("{}_sstable", level);
    return dir;
}

fs::path bloomFilterPath(fs::path dir, uint32_t level)
{
    dir /= fmt::format("{}_bloomfilter", level);
    return dir;
}

fs::path sparseIndexPath(fs::path dir, uint32_t level)
{
    dir /= fmt::format("{}_sparseindex", level);
    return dir;
}

} // namespace

LSMTree::LSMTree(
    fs::path lsmTreeDir,
    size_t maxMemtableSize,
    size_t sparseIndexFrequency)
    : lsmTreeDir_(std::move(lsmTreeDir))
    , maxMemtableSize_(maxMemtableSize)
    , sparseIndexFrequency_(sparseIndexFrequency)
{
    if (!fs::is_directory(lsmTreeDir_)) {
        throw std::runtime_error(fmt::format("{} must be a directory", lsmTreeDir_.string()));
    }

    std::set<uint32_t> levels;

    for (const auto& file : fs::directory_iterator(lsmTreeDir_)) {
        if (!fs::is_regular_file(file)) {
            throw std::runtime_error(fmt::format("{} must be a regular file", file.path().string()));
        }
        auto filename = file.path().filename().string();
        auto num = filename.substr(0, filename.find('_'));
        levels.insert(std::stoul(num));
    }

    for (auto level : levels) {
        auto sstablePath = SSTablePath(lsmTreeDir_, level);
        auto bloomfilterPath = bloomFilterPath(lsmTreeDir_, level);
        auto sparseindexPath = sparseIndexPath(lsmTreeDir_, level);

        tables_.emplace(
            level,
            structures::IndexedSSTable(sstablePath, bloomfilterPath, sparseindexPath));
    }
}

LSMTree::~LSMTree()
{
    dump();
}

std::optional<structures::Row> LSMTree::getByKey(const std::string& key) {
    auto row = memtable_.getByKey(key);
    if (row) {
        return row;
    }

    for (auto& [_, indexedSSTable] : tables_) {
        row = indexedSSTable.getByKey(key);
        if (row) {
            break;
        }
    }
    return row;
}

std::generator<structures::Row&> LSMTree::getByKeyRange(const std::string& left, const std::string& right)
{
    std::vector<std::generator<structures::Row&>> ranges;
    ranges.push_back(memtable_.getByKeyRange(left, right));
    for (auto& [_, indexedSSTable] : tables_) {
        ranges.push_back(indexedSSTable.getByKeyRange(left, right));
    }
    return merge(std::move(ranges), true);
}

std::generator<structures::Row&> LSMTree::getAll()
{
    std::vector<std::generator<structures::Row&>> ranges;
    ranges.push_back(memtable_.getAll());
    for (auto& [_, indexedSSTable] : tables_) {
        ranges.push_back(indexedSSTable.getAll());
    }
    return merge(std::move(ranges), true);
}

void LSMTree::insert(structures::Row row) {
    memtable_.insert(std::move(row));
    if (memtable_.table().size() >= maxMemtableSize_) {
        dump();
    }
}

void LSMTree::insert(std::generator<structures::Row&> rows)
{
    for (auto& row: rows) {
        insert(std::move(row));
    }
}

void LSMTree::erase(std::string key)
{
    insert(structures::Row(std::move(key), {}));
}

void LSMTree::erase(std::generator<std::string&> keys)
{
    for (auto& key: keys) {
        erase(std::move(key));
    }
}

void LSMTree::dump()
{
    if (memtable_.table().empty()) {
        return;
    }

    if (!tables_.contains(0)) {
        dump(memtable_.getAll(), memtable_.table().size(), 0);
        memtable_ = structures::MemTable();
        return;
    }
    std::vector<std::generator<structures::Row&>> ranges;
    ranges.push_back(memtable_.getAll());
    uint32_t level = 0;
    while (tables_.contains(level)) {
        ranges.push_back(tables_.at(level).getAll());
        ++level;
    }
    bool removeTombstones = tables_.lower_bound(level) == tables_.end();

    dump(merge(std::move(ranges), removeTombstones), maxMemtableSize_, level);

    for (uint32_t i = 0; i < level; ++i) {
        tables_.erase(i);

        fs::remove(SSTablePath(lsmTreeDir_, i));
        fs::remove(bloomFilterPath(lsmTreeDir_, i));
        fs::remove(sparseIndexPath(lsmTreeDir_, i));
    }
    memtable_ = structures::MemTable();
}

void LSMTree::dump(std::generator<structures::Row&> rows, size_t expectedKeysCount, uint32_t level)
{
    structures::BloomFilter bloomfilter(expectedKeysCount);
    structures::SparseIndex sparseIndex;

    std::ofstream sstableOffsteam(SSTablePath(lsmTreeDir_, level), std::ios_base::binary);

    auto offsets = structures::writeSSTableWithOffsets(sstableOffsteam, [&] -> std::generator<structures::Row&> {
        for (auto& row : rows) {
            bloomfilter.put(row.key());
            co_yield row;
        }
    }());


    size_t index = 0;
    for (auto& [key, offset] : offsets) {
        ++index;
        if (index % sparseIndexFrequency_ == 0) {
            sparseIndex.emplace(key, offset);
        }
    }
    std::ofstream bloomFilterStream(bloomFilterPath(lsmTreeDir_, level), std::ios_base::binary);
    structures::marshal::toStream(bloomfilter, bloomFilterStream);

    std::ofstream sparseIndexStream(sparseIndexPath(lsmTreeDir_, level), std::ios_base::binary);
    structures::marshal::toStream(sparseIndex, sparseIndexStream);

    sparseIndexStream.close();
    bloomFilterStream.close();
    sstableOffsteam.close();

    tables_.emplace(level, structures::IndexedSSTable(
        SSTablePath(lsmTreeDir_, level),
        bloomFilterPath(lsmTreeDir_, level),
        sparseIndexPath(lsmTreeDir_, level)));
}

} // namespace lsm
