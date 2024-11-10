#include <temporal_index/temporal_index.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <optional>

namespace temporal_index {

namespace {

using namespace fmt::literals;

std::string bitKey(TemporalIndex::TimestampType type, uint64_t bitPos, bool bitValue) {
    switch (type) {
        case TemporalIndex::TimestampType::Begin : {
            return fmt::format("_begin_{bitPos}_{bitValue}",
                "bitPos"_a = bitPos,
                "bitValue"_a = bitValue);
        }
        case TemporalIndex::TimestampType::End : {
            return fmt::format("_end_{bitPos}_{bitValue}",
                "bitPos"_a = bitPos,
                "bitValue"_a = bitValue);
        }
    }
}

} // namespace

void TemporalIndex::insertTimestamp(uint64_t key, uint64_t ts, TimestampType type)
{
    for (size_t bitPos = 0; bitPos < sizeof(ts) * 8; ++bitPos) {
        insert(bitKey(type, bitPos, ts & (1ul << bitPos)), key);
    }
}

roaring::Roaring64Map TemporalIndex::getWithGreaterEqualTimestamp(uint64_t ts, TimestampType type)
{
    roaring::Roaring64Map result;
    std::optional<roaring::Roaring64Map> disjunction;
    for (size_t bitPos = sizeof(ts) * 8 - 1; bitPos < sizeof(ts) * 8; --bitPos) {
        bool bitValue = ts & (1ul << bitPos);
        if (!bitValue) {
            if (disjunction) {
                result |= *disjunction & getByKey(bitKey(type, bitPos, true));
            } else {
                result |= getByKey(bitKey(type, bitPos, true));
            }
            continue;
        }
        if (disjunction) {
            *disjunction &= getByKey(bitKey(type, bitPos, true));
        } else {
            disjunction = getByKey(bitKey(type, bitPos, true));
        }
    }
    if (disjunction) {
        result |= *disjunction;
    }
    return result;
}

roaring::Roaring64Map TemporalIndex::getWithLessEqualTimestamp(uint64_t ts, TimestampType type)
{
    roaring::Roaring64Map result;
    std::optional<roaring::Roaring64Map> disjunction;
    for (size_t bitPos = sizeof(ts) * 8 - 1; bitPos < sizeof(ts) * 8; --bitPos) {
        bool bitValue = ts & (1ul << bitPos);
        if (bitValue) {
            if (disjunction) {
                result |= *disjunction & getByKey(bitKey(type, bitPos, false));
            } else {
                result |= getByKey(bitKey(type, bitPos, false));
            }
            continue;
        }
        if (disjunction) {
            *disjunction &= getByKey(bitKey(type, bitPos, false));
        } else {
            disjunction = getByKey(bitKey(type, bitPos, false));
        }
    }
    if (disjunction) {
        result |= *disjunction;
    }
    return result;
}

} // namespace temporal_index
