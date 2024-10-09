#include <algorithm>
#include <fixture.hpp>

#include <gtest/gtest.h>
#include <ranges>
#include <utility>

TEST_F(LSMFixture, memtable_get_by_key)
{
    lsmTree().insert(generateRows(100));

    for (auto& row : generateRows(100)) {
        auto lsmRow = lsmTree().getByKey(row.key());
        ASSERT_TRUE(lsmRow.has_value());
        ASSERT_EQ(lsmRow->key(), row.key());
        ASSERT_EQ(lsmRow->value(), row.value());
    }
}

TEST_F(LSMFixture, memtable_get_all)
{
    lsmTree().insert(generateRows(100));

    std::vector<lsm::Row> rows;
    for (auto& row : generateRows(100)) {
        rows.push_back(std::move(row));
    }

    size_t i = 0;
    for (auto& lsmRow : lsmTree().getAll()) {
        ASSERT_EQ(lsmRow.key(), rows[i].key());
        ASSERT_EQ(lsmRow.value(), rows[i].value());
        ++i;
    }
}

TEST_F(LSMFixture, memtable_get_by_range)
{
    lsmTree().insert(generateRows(100));

    std::vector<lsm::Row> rows;
    for (auto& row : generateRows(100)) {
        rows.push_back(std::move(row));
    }

    size_t i = 50;
    for (auto& lsmRow : lsmTree().getByKeyRange(rows[i].key(), rows.back().key())) {
        ASSERT_EQ(lsmRow.key(), rows[i].key());
        ASSERT_EQ(lsmRow.value(), rows[i].value());
        ++i;
    }
}


TEST_F(LSMFixture, memtable_erase)
{
    lsmTree().insert(lsm::Row("1", "1"));
    ASSERT_TRUE(lsmTree().getByKey("1").has_value());
    lsmTree().erase("1");
    ASSERT_FALSE(lsmTree().getByKey("1").has_value());
}
