#include "fmt/base.h"
#include <fixture.hpp>
#include <temporal_index/filter.hpp>

using enum temporal_index::TemporalIndex::TimestampType;

TEST_F(TemporalIndexFixture, simple)
{
    for (size_t i = 1; i <= 100; ++i) {
        index().insertTimestamp(i, i, Begin);
    }
    for (size_t i = 1; i <= 1000; ++i) {
        auto bitmap = index().getWithLessEqualTimestamp(i, Begin);
        ASSERT_EQ(bitmap.cardinality(), std::min(i, 100ul));
        for (size_t j = 1; j <= std::min(i, 100ul); ++j) {
            ASSERT_TRUE(bitmap.contains(j));
        }
    }
    for (size_t i = 1; i <= 1000; ++i) {
        auto bitmap = index().getWithGreaterEqualTimestamp(i, Begin);
        ASSERT_EQ(bitmap.cardinality(), 101 - std::min(i, 101ul));
        for (size_t j = i; j <= 100ul; ++j) {
            ASSERT_TRUE(bitmap.contains(j));
        }
    }
}

TEST_F(TemporalIndexFixture, filter)
{
    for (size_t i = 1; i <= 100; ++i) {
        index().insertTimestamp(i, i, Begin);
        index().insertTimestamp(i, i + 50, End);
    }
    for (size_t i = 1; i <= 100; ++i) {
        for (size_t j = i; j <= 100; ++j) {
            auto bitmap = temporal_index::filter::createdBetween(i, j).exec(index());
            ASSERT_EQ(bitmap.cardinality(), j - i + 1);
        }
    }
    auto validRange = [] (int64_t begin, int64_t end) {
        int64_t created = std::min(begin, 100l);
        int64_t deleted = std::min(std::max(0l, end - 50), created);
        int64_t start = 1;
        start = std::max(start, end - 50 + 1);

        return std::make_pair(start, start + (created - deleted));
    };

    for (size_t i = 1; i <= 150; ++i) {
        for (size_t j = i; j <= 150; ++j) {
            auto bitmap = temporal_index::filter::validBetween(i, j).exec(index());
            auto [left, right] = validRange(i, j);
            ASSERT_EQ(bitmap.cardinality(), right - left);
            for (size_t pos = left; pos < right; ++pos) {
                ASSERT_TRUE(bitmap.contains(pos));
            }
        }
    }
}
