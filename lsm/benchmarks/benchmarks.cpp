#include "benchmark/benchmark.h"
#include <fixtures.hpp>

using LSMFixture0 = LSMFixture<10'000, 100, 0, 1000>;
BENCHMARK_DEFINE_F(LSMFixture0, Insert)(benchmark::State& state)
{
    static constexpr size_t rowsCount = 1'000'000;
    state.ResumeTiming();
    lsmTree().insert(generateRows(rowsCount));
    state.PauseTiming();
    state.SetBytesProcessed(rowsCount * LSMFixture0::RowSize);

}
BENCHMARK_REGISTER_F(LSMFixture0, Insert)->Unit(benchmark::kSecond);

using LSMFixture10000000 = LSMFixture<10'000, 100, 1'000'000, 1000>;
BENCHMARK_DEFINE_F(LSMFixture10000000, GetByKey)(benchmark::State& state)
{
    size_t rowsProcessed = 0;
    state.ResumeTiming();
    while (rowsProcessed < 100'000) {
        auto row = lsmTree().getByKey(randKey());
        ++rowsProcessed;
    }

    state.PauseTiming();
    state.SetBytesProcessed(rowsProcessed * LSMFixture10000000::RowSize);
}
BENCHMARK_REGISTER_F(LSMFixture10000000, GetByKey)->Unit(benchmark::kSecond);

BENCHMARK_DEFINE_F(LSMFixture10000000, GetByKeyRange)(benchmark::State& state)
{
    size_t rowsProcessed = 0;
    state.ResumeTiming();
    while (rowsProcessed < 10'000'000) {
        auto range = randKeyRange();
        auto rows = lsmTree().getByKeyRange(range.first, range.second);
        for (auto row : rows) {
            ++rowsProcessed;
        }
    }
    state.PauseTiming();
    state.SetBytesProcessed(rowsProcessed * LSMFixture10000000::RowSize);
}
BENCHMARK_REGISTER_F(LSMFixture10000000, GetByKeyRange)->Unit(benchmark::kSecond);

BENCHMARK_DEFINE_F(LSMFixture10000000, GetAll)(benchmark::State& state)
{
    size_t rowsProcessed = 0;
    state.ResumeTiming();
    while (rowsProcessed < 10'000'000) {
        auto row = lsmTree().getAll();
        for (auto _ : row) {
            ++rowsProcessed;
        }
    }
    state.PauseTiming();
    state.SetBytesProcessed(rowsProcessed * LSMFixture10000000::RowSize);
}
BENCHMARK_REGISTER_F(LSMFixture10000000, GetAll)->Unit(benchmark::kSecond);

BENCHMARK_MAIN();
