#include <temporal_index/temporal_index.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <optional>

namespace temporal_index {

namespace {

using namespace fmt::literals;

std::string bitKey(TemporalIndex::TimestampType type, uint64_t bitPos) {
    switch (type) {
        case TemporalIndex::TimestampType::Begin : {
            return fmt::format("_begin_{bitPos}",
                "bitPos"_a = bitPos);
        }
        case TemporalIndex::TimestampType::End : {
            return fmt::format("_end_{bitPos}",
                "bitPos"_a = bitPos);
        }
    }
}

std::string allKey(TemporalIndex::TimestampType type) {
    static const std::string BEGIN_ALL = "_begin_all";
    static const std::string END_ALL = "_end_all";

    switch (type) {
        case TemporalIndex::TimestampType::Begin : {
            return BEGIN_ALL;
        }
        case TemporalIndex::TimestampType::End : {
            return END_ALL;
        }
    }
}

} // namespace

void TemporalIndex::insertTimestamp(uint64_t key, uint64_t ts, TimestampType type)
{
    insert(allKey(type), key);
    for (size_t bitPos = 0; bitPos < sizeof(ts) * 8; ++bitPos) {
        if (ts & (1ul << bitPos)) {
            insert(bitKey(type, bitPos), key);
        }
    }
}

roaring::Roaring64Map TemporalIndex::getWithGreaterEqualTimestamp(uint64_t ts, TimestampType type)
{
    roaring::Roaring64Map result;
    std::optional<roaring::Roaring64Map> conjunction;
    for (size_t bitPos = sizeof(ts) * 8 - 1; bitPos < sizeof(ts) * 8; --bitPos) {
        bool bitValue = ts & (1ul << bitPos);
        if (!bitValue) {
            if (conjunction) {
                result |= *conjunction & getByKey(bitKey(type, bitPos));
            } else {
                result |= getByKey(bitKey(type, bitPos));
            }
            continue;
        }
        if (conjunction) {
            *conjunction &= getByKey(bitKey(type, bitPos));
        } else {
            conjunction = getByKey(bitKey(type, bitPos));
        }
    }
    if (conjunction) {
        result |= *conjunction;
    }
    return result;
}

roaring::Roaring64Map TemporalIndex::getWithLessEqualTimestamp(uint64_t ts, TimestampType type)
{
    roaring::Roaring64Map all = getByKey(allKey(type));
    roaring::Roaring64Map result;
    std::optional<roaring::Roaring64Map> conjunction;
    for (size_t bitPos = sizeof(ts) * 8 - 1; bitPos < sizeof(ts) * 8; --bitPos) {
        bool bitValue = ts & (1ul << bitPos);
        if (bitValue) {
            if (conjunction) {
                result |= *conjunction & (all - getByKey(bitKey(type, bitPos)));
            } else {
                result |= all - getByKey(bitKey(type, bitPos));
            }
            continue;
        }
        if (conjunction) {
            *conjunction &= all - getByKey(bitKey(type, bitPos));
        } else {
            conjunction = all - getByKey(bitKey(type, bitPos));
        }
    }
    if (conjunction) {
        result |= *conjunction;
    }
    return result;
}

} // namespace temporal_index
