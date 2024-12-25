#include <kd_tree/kd_tree.hpp>
#include <benchmark/benchmark.h>
#include <random>

using namespace kd_tree;

constexpr static double MAX = 1000;

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

void benchmarkContains(benchmark::State& st)
{
    auto k = st.range(0);
    auto n = st.range(1);
    auto vectors = generateVectors(n, k);
    kd_tree::KDTree tree(vectors);
    size_t i = 0;
    while (st.KeepRunning()) {
        tree.contains(vectors[i]);
        i = (i + 1) % vectors.size();
    }
}
BENCHMARK(benchmarkContains)->RangeMultiplier(10)->Ranges({{1, 1000}, {10000, 1000000}});


void benchmarkNearest(benchmark::State& st)
{
    auto k = st.range(0);
    auto n = st.range(1);
    auto vectors = generateVectors(n, k);
    kd_tree::KDTree tree(vectors);
    size_t i = 0;
    while (st.KeepRunning()) {
        tree.findNearest(vectors[i]);
        i = (i + 1) % vectors.size();
    }
}
BENCHMARK(benchmarkNearest)->RangeMultiplier(10)->Ranges({{1, 1000}, {10000, 1000000}});

BENCHMARK_MAIN();
