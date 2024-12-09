#include <fm_index/fm_index.hpp>
#include <fm_index/bwt.hpp>
#include <gtest/gtest.h>

#include <string>
#include <random>

using namespace fm_index;
using namespace std::literals;

namespace {

std::string randomString(size_t size)
{
    const auto alphabet = "abcdefg"s;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    std::string res;
    for (size_t _ = 0; _ < size; ++_) {
        res.push_back(alphabet[dist(gen)]);
    }
    return res;
}

std::vector<size_t> bruteforcePatterns(const std::string& text, const std::string pattern)
{
    std::vector<size_t> res;
    size_t pos = 0;
    while (true) {
        pos = text.find(pattern, pos);
        if (pos != std::string::npos) {
            res.push_back(pos);
            ++pos;
        } else {
            break;
        }
    }
    return res;
}

} // namespace

TEST(bwt, tudasuda)
{
    for (size_t iter = 0; iter < 1000; ++iter) {
        auto s = randomString(100);
        auto transformed = bwt::directTransform(s);
        auto inversed = bwt::inverseTransform(transformed);
        ASSERT_EQ(inversed, s);
    }
}

TEST(fm_index, count)
{
    for (size_t iter = 0; iter < 10; ++iter) {
        auto s = randomString(50);
        FmIndex index(s);
        for (size_t l = 0; l < s.size(); ++l) {
            for (size_t r = l + 1; r < s.size(); ++r) {
                auto substr = s.substr(l, r - l);
                ASSERT_EQ(bruteforcePatterns(s, substr).size(), index.count(substr));
            }
        }
    }
}

TEST(fm_index, find)
{
    for (size_t iter = 0; iter < 10; ++iter) {
        auto s = randomString(50);
        FmIndex index(s);
        for (size_t l = 0; l < s.size(); ++l) {
            for (size_t r = l + 1; r < s.size(); ++r) {
                auto substr = s.substr(l, r - l);
                ASSERT_EQ(bruteforcePatterns(s, substr), index.find(substr));
            }
        }
    }
}
