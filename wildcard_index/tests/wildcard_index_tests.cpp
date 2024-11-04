#include <cstdint>
#include <fixture.hpp>
#include <wildcard_index/tokenize.hpp>

constexpr auto SIMPLE_DOC = "To control system parameters, open Settings. Dry clothes can be taken out of the drier. He is a moving man.";

using namespace wildcard_index;

TEST_F(WildcardIndexFixture, simple)
{
    index().insertDoc(42, SIMPLE_DOC);
    auto testWildcard = [&](const std::string& wildcard, const std::vector<uint64_t>& docIds) {
        auto bitmap = index().getByWildcard(wildcard);
        bool good = bitmap.cardinality() == docIds.size();
        for (auto id : docIds) {
            good |= bitmap.contains(id);
        }
        if (!good) {
            bitmap.printf();
        }
        return good;
    };
    ASSERT_TRUE(testWildcard("control", {42}));
    ASSERT_TRUE(testWildcard("c*ntrol", {42}));
    ASSERT_TRUE(testWildcard("c*trol", {42}));
    ASSERT_TRUE(testWildcard("c*rol", {42}));
    ASSERT_TRUE(testWildcard("c*ol", {42}));
    ASSERT_TRUE(testWildcard("c*l", {42}));
    ASSERT_TRUE(testWildcard("c*", {42}));
    ASSERT_TRUE(testWildcard("*", {42}));
    ASSERT_TRUE(testWildcard("*l", {42}));
    ASSERT_TRUE(testWildcard("*ol", {42}));
    ASSERT_TRUE(testWildcard("*rol", {42}));
    ASSERT_TRUE(testWildcard("*trol", {42}));
    index().dump();
    index().insertDoc(4242, SIMPLE_DOC);
    ASSERT_TRUE(testWildcard("control", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*ntrol", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*trol", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*rol", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*ol", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*l", {42, 4242}));
    ASSERT_TRUE(testWildcard("c*", {42, 4242}));
    ASSERT_TRUE(testWildcard("*", {42, 4242}));
    ASSERT_TRUE(testWildcard("*l", {42, 4242}));
    ASSERT_TRUE(testWildcard("*ol", {42, 4242}));
    ASSERT_TRUE(testWildcard("*rol", {42, 4242}));
    ASSERT_TRUE(testWildcard("*trol", {42, 4242}));
}
