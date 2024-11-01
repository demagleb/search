#include <fixture.hpp>
#include <inverted_index/tokenize.hpp>

#include <array>

constexpr auto SIMPLE_DOC = "To control system parameters, open Settings. Dry clothes can be taken out of the drier. He is a moving man.";
constexpr auto SIMPLE_WORD = "Settings";

constexpr std::array DOCS = {
    "The sun set behind the hills, painting the sky with shades of orange and pink. A perfect evening.",
    "She opened the old book; its pages whispered secrets of the past, long forgotten but not lost.",
    "Music filled the room, and every note seemed to dance in the air, creating a serene harmony.",
    "The city lights blinked in the distance as cars rushed by, a symphony of life and energy.",
    "His laughter was infectious, spreading joy to everyone around, a spark of happiness in the night.",
    "A gentle breeze rustled the leaves, carrying whispers of autumn through the quiet forest.",
    "They stood at the shore, waves kissing their feet, dreams cast upon the endless blue horizon.",
    "Her eyes sparkled like stars, windows to her soul, profound and full of untold stories.",
    "A cozy cafe corner, the aroma of coffee and soft chatter, a sanctuary from the world outside.",
    "The clock ticked softly, marking the moments in a silent room, time's endless march forward."
} ;

using namespace inverted_index;

TEST(unit, tokenize)
{
    std::vector<std::string> tokens = {"control", "system", "paramet", "open", "set", "dri", "cloth", "taken", "drier", "move", "man"};
    size_t i = 0;
    for (auto& word : tokenize(SIMPLE_DOC)) {
        ASSERT_LE(i, tokens.size());
        ASSERT_EQ(word, tokens[i]);
        ++i;
    }
}

TEST_F(InvertedIndexFixture, simple)
{
    for (auto& word : tokenize(SIMPLE_DOC)) {
        ASSERT_EQ(index().getByWord(word).cardinality(), 0);
    }
    index().insertDoc(42, SIMPLE_DOC);
    {
        auto values = index().getByWord(SIMPLE_WORD);
        ASSERT_EQ(values.cardinality(), 1);
        ASSERT_TRUE(values.contains(42ul));
    }
    for (auto& word : tokenize(SIMPLE_DOC)) {
        auto values = index().getByWord(word);
        ASSERT_EQ(values.cardinality(), 1);
        ASSERT_TRUE(values.contains(42ul));
    }
    for (auto& word : tokenize(SIMPLE_DOC)) {
        auto values = index().getByWord(word);
        ASSERT_EQ(values.cardinality(), 1);
        ASSERT_TRUE(values.contains(42ul));
    }
    index().insertDoc(4242, SIMPLE_DOC);
    for (auto& word : tokenize(SIMPLE_DOC)) {
        auto values = index().getByWord(word);
        ASSERT_EQ(values.cardinality(), 2);
        ASSERT_TRUE(values.contains(42ul));
        ASSERT_TRUE(values.contains(4242ul));
    }
}

TEST_F(InvertedIndexFixture, many_docs)
{
    static constexpr size_t ITERS = 100;
    for (size_t k = 0; k < ITERS; ++k) {
        for (size_t i = 0; i < DOCS.size(); ++i) {
            index().insertDoc(i * ITERS + k, DOCS[i]);
        }
        index().dump();
    }

    for (size_t i = 0; i < DOCS.size(); ++i) {
        for (auto& word : tokenize(DOCS[i])) {
            auto values = index().getByWord(word);
            ASSERT_GE(values.cardinality(), ITERS);
            for (size_t k = 0; k < ITERS; ++k) {
                ASSERT_TRUE(values.contains(i * ITERS + k));
            }
        }
    }
}
