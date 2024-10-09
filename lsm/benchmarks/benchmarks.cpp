#include "benchmark/benchmark.h"
#include <fixtures.hpp>

using LSMFixture0 = LSMFixture<0>;
BENCHMARK_DEFINE_F(LSMFixture0, Insert)(benchmark::State& state) {
    // Insert 0.6GB
    lsmTree().insert(generateRows(10'000'000));
}
BENCHMARK_REGISTER_F(LSMFixture0, Insert)->Unit(benchmark::kSecond)->Iterations(1);

BENCHMARK_MAIN();
