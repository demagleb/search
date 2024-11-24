#pragma once

#include <cstdint>
#include <lsm/lsm.hpp>
#include <roaring64map.hh>
#include <positional_index/structures/postings_list.hpp>
#include <set>

namespace positional_index {

namespace {

namespace fs = std::filesystem;

} // namespace

class PositionalIndex {
public:
    PositionalIndex(fs::path dir);

    virtual ~PositionalIndex();

    void insert(const std::string& key, uint64_t value, uint64_t position);

    std::generator<structures::Posting&> getByKey(std::string key);
    std::generator<std::generator<structures::Posting&>&> getByKeyRange(std::string left, std::string right);

    roaring::Roaring64Map getWithSequence(const std::vector<std::string>& sequence, uint8_t tolerance = 0);

    void dump();

    void finish();
private:
    bool finished_ = false;
    lsm::LSMTree lsmTree_;
    std::map<std::string, std::set<structures::Posting>> memtable_;
};

} // namespace positional_index
