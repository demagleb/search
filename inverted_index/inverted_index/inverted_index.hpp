#pragma once

#include <lsm/lsm.hpp>
#include <roaring64map.hh>

#include <unordered_map>

namespace inverted_index {

namespace {

namespace fs = std::filesystem;

} // namespace

class InvertedIndex {
public:
    InvertedIndex(fs::path dir);

    ~InvertedIndex();

    void insert(uint64_t key, const std::string& doc);

    roaring::Roaring64Map getByWord(std::string word);

    void dump();

    void finish();
private:
    bool finished_ = false;
    lsm::LSMTree lsmTree_;
    std::unordered_map<std::string, roaring::Roaring64Map> memtable_;
};

} // namespace inverted_index
