#pragma once

#include <bitmap_index/bitmap_index.hpp>

namespace temporal_index {

class TemporalIndex : public bitmap_index::BitmapIndex {
public:
    enum class TimestampType {
        Begin,
        End,
    };

    TemporalIndex(std::filesystem::path dir)
        : bitmap_index::BitmapIndex(std::move(dir))
    { }

    void insertTimestamp(uint64_t key, uint64_t ts, TimestampType type);

    roaring::Roaring64Map getWithGreaterEqualTimestamp(uint64_t ts, TimestampType type);
    roaring::Roaring64Map getWithLessEqualTimestamp(uint64_t ts, TimestampType type);
};

} // namespace temporal_index
