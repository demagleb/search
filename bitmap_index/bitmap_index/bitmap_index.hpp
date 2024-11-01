#pragma once

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

#include <unordered_map>

namespace bitmap_index {

namespace {

namespace fs = std::filesystem;

} // namespace

class BitmapIndex {
public:
    BitmapIndex(fs::path dir);

    ~BitmapIndex();

    void insert(const std::string& key, uint64_t value);

    roaring::Roaring64Map get(const std::string& key);

    void dump();

    void finish();
private:
    bool finished_ = false;
    lsm::LSMTree lsmTree_;
    std::unordered_map<std::string, roaring::Roaring64Map> memtable_;
};

} // namespace bitmap_index
