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
            structures::IndexedSSTable(sstablePath, bloomfilterPath, sstablePath));
    }
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
        ranges.push_back(memtable_.getByKeyRange(left, right));
    }
    return merge(std::move(ranges));
}

void LSMTree::insert(std::generator<structures::Row&> rows)
{
    for (auto& row: rows) {
        memtable_.insert(row);
        if (memtable_.table().size() >= maxMemtableSize_) {
            dump();
        }
    }
}

void LSMTree::dump()
{
    if (!tables_.contains(0)) {
        dump(memtable_.getAll(), memtable_.table().size(), 0);
    }
    std::vector<std::generator<structures::Row&>> ranges;
    ranges.push_back(memtable_.getAll());
    uint32_t level = 0;
    while (tables_.contains(level)) {
        ranges.push_back(tables_.at(level).getAll());
    }
    dump(merge(std::move(ranges)), memtable_.table().size(), level);
    for (uint32_t i = 0; i < level; ++i) {
        tables_.erase(i);

        fs::remove(SSTablePath(lsmTreeDir_, i));
        fs::remove(bloomFilterPath(lsmTreeDir_, i));
        fs::remove(sparseIndexPath(lsmTreeDir_, i));
    }
}

void LSMTree::dump(std::generator<structures::Row&> rows, size_t expectedKeysCount, uint32_t level)
{
    structures::BloomFilter bloomfilter(expectedKeysCount);
    structures::SparseIndex sparseIndex;

    std::ofstream sstableOffsteam(SSTablePath(lsmTreeDir_, level));

    auto offsets = structures::writeSSTableWithOffsets(sstableOffsteam, [&] -> std::generator<structures::Row&> {
        for (auto& row : rows) {
            bloomfilter.put(row.key());
            co_yield row;
        }
    }());

    size_t index = 0;
    for (auto& [key, offset] : offsets) {
        ++index;
        if (index % sparseIndexFrequency_) {
            sparseIndex.emplace(key, offset);
        }
    }
    std::ofstream bloomFilterStream(bloomFilterPath(lsmTreeDir_, level));
    structures::marshal::toStream(bloomfilter, bloomFilterStream);

    std::ofstream sparseIndexStream(sparseIndexPath(lsmTreeDir_, level));
    structures::marshal::toStream(sparseIndex, sparseIndexStream);
}

} // namespace lsm
