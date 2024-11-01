#include <fixture.hpp>

TEST_F(BitmapIndexFixture, simple) {
    index().insert("key", 1);
    ASSERT_EQ(index().get("key").cardinality(), 1);
    ASSERT_TRUE(index().get("key").contains(1ul));

    index().dump();
    ASSERT_EQ(index().get("key").cardinality(), 1);
    ASSERT_TRUE(index().get("key").contains(1ul));

    index().insert("key", 2);
    ASSERT_EQ(index().get("key").cardinality(), 2);
    ASSERT_TRUE(index().get("key").contains(1ul));
    ASSERT_TRUE(index().get("key").contains(2ul));

    index().dump();
    ASSERT_EQ(index().get("key").cardinality(), 2);
    ASSERT_TRUE(index().get("key").contains(1ul));
    ASSERT_TRUE(index().get("key").contains(2ul));
}
