#pragma once

#include <lsm/structures/marshal.hpp>

#include <cstdint>
#include <vector>
#include <generator>

namespace lsm::structures {

class BloomFilter {
public:
    BloomFilter(std::vector<std::byte> filter, std::vector<uint64_t> hashSeeds)
        : filter_(std::move(filter))
        , hashSeeds_(std::move(hashSeeds))
    { }

    BloomFilter(size_t expectedKeysCount);

    bool hasValueFalsePositive(std::string_view key) const;
    void put(std::string_view key);

    const std::vector<std::byte>& filter() const { return filter_; };
    const std::vector<uint64_t>& hashSeeds() const { return hashSeeds_; };
private:
    std::vector<std::byte> filter_;
    const std::vector<uint64_t> hashSeeds_;
};

namespace marshal::impl {

template <>
BloomFilter fromStreamImpl<BloomFilter>(std::istream& istream);

template <>
std::streamoff toStreamImpl<BloomFilter>(const BloomFilter& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
