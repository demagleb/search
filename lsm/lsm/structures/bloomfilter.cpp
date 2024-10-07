#include <lsm/structures/bloomfilter.hpp>

#include <rapidhash.h>

#include <cstdint>
#include <string_view>
#include <utility>

namespace lsm::structures {

namespace {

const std::vector<uint64_t> DEFAULT_HASH_SEEDS = {
    4190103122858907167ul,
    5187983211337934731ul,
    1522309665872014886ul,
};

} // namespace

BloomFilter::BloomFilter(size_t expectedKeysCount)
    : hashSeeds_(DEFAULT_HASH_SEEDS)
{
    filter_.resize(100 + expectedKeysCount / 8, std::byte{0});
}

bool BloomFilter::hasValueFalsePositive(std::string_view key) const
{
    for (auto seed : hashSeeds_) {
        auto hash = rapidhash_withSeed(key.data(), key.size(), seed) % filter_.size();
        auto positive = filter_[hash / 8] & static_cast<std::byte>(1 << (hash & 7ul));
        if (positive == std::byte{0}) {
            // std::cerr << "boomfilter do not have \"" << key << "\"" << std::endl;
            return false;
        }
    }
    // std::cerr << "boomfilter have \"" << key << "\"" << std::endl;
    return true;
}

void BloomFilter::put(std::string_view key)
{
    for (auto seed : hashSeeds_) {
        auto hash = rapidhash_withSeed(key.data(), key.size(), seed) % filter_.size();
        filter_[hash / 8] |= static_cast<std::byte>(1 << (hash & 7ul));
    }
}

namespace marshal::impl {

template <>
BloomFilter fromStreamImpl<BloomFilter>(std::istream& istream)
{
    size_t hashSeedsSize = 0;
    istream.read(reinterpret_cast<char*>(&hashSeedsSize), sizeof(hashSeedsSize));
    std::vector<uint64_t> hashSeeds(hashSeedsSize);
    istream.read(reinterpret_cast<char*>(hashSeeds.data()), hashSeedsSize * sizeof(hashSeeds[0]));

    size_t filterSize = 0;
    istream.read(reinterpret_cast<char*>(&filterSize), sizeof(filterSize));
    std::vector<std::byte> filter(filterSize);
    istream.read(reinterpret_cast<char*>(filter.data()), filterSize * sizeof(filter[0]));

    return BloomFilter(std::move(filter), std::move(hashSeeds));
}

template <>
std::streamoff toStreamImpl<BloomFilter>(const BloomFilter& value, std::ostream& ostream)
{
    auto pos = ostream.tellp();
    size_t hashSeedsSize = value.hashSeeds().size();
    ostream << std::string_view(
        reinterpret_cast<const char*>(&hashSeedsSize),
        sizeof(hashSeedsSize));
    ostream << std::string_view(
        reinterpret_cast<const char*>(value.hashSeeds().data()),
        value.hashSeeds().size() * sizeof(value.hashSeeds()[0]));

    size_t filterSize = value.filter().size();
    ostream << std::string_view(
        reinterpret_cast<const char*>(&filterSize),
        sizeof(filterSize));
    ostream << std::string_view(
        reinterpret_cast<const char*>(value.filter().data()),
        value.filter().size() * sizeof(value.filter()[0]));
    return pos;
}

} // namespace marshal::impl

} // namespace lsm::structures
