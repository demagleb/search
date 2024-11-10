#pragma once

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

namespace bitmap_index {

namespace {

namespace fs = std::filesystem;

} // namespace

class BitmapIndex {
public:
    BitmapIndex(fs::path dir);

    virtual ~BitmapIndex();

    void insert(const std::string& key, uint64_t value);

    roaring::Roaring64Map getByKey(const std::string& key);
    std::generator<roaring::Roaring64Map&> getByKeyRange(std::string left, std::string right);

    void dump();

    void finish();
private:
    bool finished_ = false;
    lsm::LSMTree lsmTree_;
    std::map<std::string, roaring::Roaring64Map> memtable_;
};

} // namespace bitmap_index
