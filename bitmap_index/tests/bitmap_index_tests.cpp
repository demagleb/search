#include <fixture.hpp>
#include <bitmap_index/filter.hpp>
#include <gtest/gtest.h>

using namespace bitmap_index;

TEST_F(BitmapIndexFixture, getByKey) {
    index().insert("key", 1);
    ASSERT_EQ(index().getByKey("key").cardinality(), 1);
    ASSERT_TRUE(index().getByKey("key").contains(1ul));

    index().dump();
    ASSERT_EQ(index().getByKey("key").cardinality(), 1);
    ASSERT_TRUE(index().getByKey("key").contains(1ul));

    index().insert("key", 2);
    ASSERT_EQ(index().getByKey("key").cardinality(), 2);
    ASSERT_TRUE(index().getByKey("key").contains(1ul));
    ASSERT_TRUE(index().getByKey("key").contains(2ul));

    index().dump();
    ASSERT_EQ(index().getByKey("key").cardinality(), 2);
    ASSERT_TRUE(index().getByKey("key").contains(1ul));
    ASSERT_TRUE(index().getByKey("key").contains(2ul));
}

TEST_F(BitmapIndexFixture, getByKeyRange) {
    index().insert("key1", 1);
    index().insert("key2", 2);
    index().insert("key3", 3);
    index().insert("key6", 6);
    index().dump();
    index().insert("key1", 10);
    index().insert("key2", 20);
    index().insert("key3", 30);

    index().insert("key4", 4);
    index().insert("key4", 40);
    index().insert("key5", 5);
    index().insert("key5", 50);
    index().insert("key6", 60);
    index().insert("key7", 7);

    const std::string left = "key1";
    const std::string right = "key5";

    size_t i = 1;
    for (auto& bitmap : index().getByKeyRange(left, right)) {
        ASSERT_EQ(bitmap.cardinality(), 2);
        ASSERT_TRUE(bitmap.contains(i));
        ASSERT_TRUE(bitmap.contains(10 * i));
        ++i;
    }

    ASSERT_EQ(i, 6);
}

TEST_F(BitmapIndexFixture, haveFilter) {
    index().insert("1", 1);
    index().insert("1", 2);
    index().insert("1", 3);

    auto bitmap = filter::have("1").exec(index());

    ASSERT_EQ(bitmap.cardinality(), 3);
    ASSERT_TRUE(bitmap.contains(1ul));
    ASSERT_TRUE(bitmap.contains(2ul));
    ASSERT_TRUE(bitmap.contains(3ul));
}

TEST_F(BitmapIndexFixture, betweenFilter) {
    index().insert("key1", 1);
    index().insert("key2", 2);
    index().insert("key3", 3);
    index().insert("key4", 4);

    auto bitmap = filter::between("key1", "key3").exec(index());
    ASSERT_EQ(bitmap.cardinality(), 3);
    ASSERT_TRUE(bitmap.contains(1ul));
    ASSERT_TRUE(bitmap.contains(2ul));
    ASSERT_TRUE(bitmap.contains(3ul));
}

TEST_F(BitmapIndexFixture, filterCombinators) {
    index().insert("1", 12);
    index().insert("1", 13);
    index().insert("1", 123);

    index().insert("2", 12);
    index().insert("2", 23);
    index().insert("2", 123);

    index().insert("3", 13);
    index().insert("3", 23);
    index().insert("3", 123);


    auto filter = filter::have("1")
        & filter::have("2")
        & filter::have("3");
    auto bitmap = filter.exec(index());

    ASSERT_EQ(bitmap.cardinality(), 1);
    ASSERT_TRUE(bitmap.contains(123ul));

    filter = filter::have("1")
        & filter::have("2")
        - filter::have("3");
    bitmap = filter.exec(index());

    ASSERT_EQ(bitmap.cardinality(), 1);
    ASSERT_TRUE(bitmap.contains(12ul));

    filter = (filter::have("3") - filter::have("2"))
        | filter::have("1");

    bitmap = filter.exec(index());

    ASSERT_EQ(bitmap.cardinality(), 3);
    ASSERT_TRUE(bitmap.contains(12ul));
    ASSERT_TRUE(bitmap.contains(13ul));
    ASSERT_TRUE(bitmap.contains(123ul));
}
