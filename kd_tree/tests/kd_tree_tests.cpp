#include <kd_tree/kd_tree.hpp>
#include <gtest/gtest.h>
#include <random>


using namespace kd_tree;


namespace {

constexpr static double MAX = 1000;
constexpr double EPS = MAX * 1e-12;

std::vector<KDTree::Vector> generateVectors(size_t n, size_t k)
{
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-MAX, MAX);
    std::vector<KDTree::Vector> result(n, KDTree::Vector(k));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < k; ++j) {
            result[i][j] = dist(gen);
        }
    }
    return result;
}

std::vector<double> addEps(std::vector<double> v) {
    for (auto& x : v) {
        x += EPS;
    }
    return v;
}

} // namespace

TEST(KDTree, build)
{
    auto vectors = generateVectors(10000, 10);
    ASSERT_NO_THROW(KDTree(std::move(vectors)));
}

TEST(KDTree, contains)
{
    auto vectors = generateVectors(10000, 10);
    KDTree tree(vectors);

    for (const auto& v : vectors) {
        ASSERT_TRUE(tree.contains(v));
    }
}

TEST(KDTree, findNearest)
{
    auto vectors = generateVectors(10000, 10);
    KDTree tree(vectors);

    for (const auto& v : vectors) {
        ASSERT_EQ(tree.findNearest(v), v);
        ASSERT_EQ(tree.findNearest(addEps(v)), v);
    }
}

TEST(KDTree, insert)
{
    auto vectors = generateVectors(10000, 10);
    KDTree tree(vectors);

    for (const auto& v : vectors) {
        ASSERT_TRUE(tree.contains(v));
    }

    auto vectors2 = generateVectors(100, 10);
    for (const auto& v : vectors) {
        tree.insert(v);
        ASSERT_TRUE(tree.contains(v));
    }
}
