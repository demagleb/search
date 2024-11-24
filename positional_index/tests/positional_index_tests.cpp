#include "roaring64map.hh"
#include <fixture.hpp>

#include <fmt/format.h>

using namespace positional_index;
using namespace positional_index::structures;

TEST_F(PositionalIndexFixture, simple) {
    index().insert("1", 1, 1);
    index().insert("1", 1, 3);

    index().insert("2", 1, 2);
    index().insert("2", 1, 4);
    index().dump();
    index().insert("1", 2, 1);
    index().insert("2", 2, 2);


    std::vector<std::vector<Posting>> good = {
    {
            Posting(1, {1, 2}),
            Posting(3, {1}),
        },
    {
            Posting(2, {1, 2}),
            Posting(4, {1}),
        }
    };

    size_t pos = 0;
    for (auto& posting : index().getByKey("1")) {
        ASSERT_EQ(posting.position(), good[0][pos].position());
        ASSERT_EQ(posting.bitmap(), good[0][pos].bitmap());
        ++pos;
    }
    pos = 0;
    for (auto& posting : index().getByKey("2")) {
        ASSERT_EQ(posting.position(), good[1][pos].position());
        ASSERT_EQ(posting.bitmap(), good[1][pos].bitmap());
        ++pos;
    }
    size_t listPos = 0;
    for (auto& postingsList : index().getByKeyRange("1", "2")) {
        pos = 0;
        for (auto& posting : postingsList) {
            ASSERT_EQ(posting.position(), good[listPos][pos].position());
            ASSERT_EQ(posting.bitmap(), good[listPos][pos].bitmap());
            ++pos;
        }
        ++listPos;
    }
}

TEST_F(PositionalIndexFixture, hasSequence) {
    const std::vector<std::vector<std::string>> docs = {
        {"cat", "sat", "on", "the", "mat"},
        {"dog", "barked", "at", "the", "cat"},
        {"sun", "shines", "over", "the", "hills"},
        {"cat", "chased", "the", "dog", "away"},
        {"hills", "are", "very", "green", "today"},
        {"sun", "sets", "over", "the", "hills"},
        {"dog", "sleeps", "on", "the", "mat"},
        {"cat", "and", "dog", "are", "tired"},
        {"hills", "echo", "with", "the", "sound"},
        {"sun", "rises", "brightly", "every", "day"},
    };

    for (size_t docId = 0; docId < docs.size(); ++docId) {
        for (size_t position = 0; position < docs[docId].size(); ++position) {
            index().insert(docs[docId][position], docId, position);
        }
    }

    for (size_t docId = 0; docId < docs.size(); ++docId) {
        ASSERT_EQ(index().getWithSequence(docs[docId]), roaring::Roaring64Map{docId});
    }
    auto got = index().getWithSequence({"cat", "dog"}, 2);
    auto need = roaring::Roaring64Map{3, 7};
    ASSERT_EQ(got, need);
    got = index().getWithSequence({"on", "the"});
    need = roaring::Roaring64Map{0, 6};
    ASSERT_EQ(got, need);

}
