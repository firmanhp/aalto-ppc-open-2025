#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <chrono>

static void BM_CacheSize(benchmark::State& state) {
    const size_t bytes = state.range(0);
    const size_t count = bytes / sizeof(size_t);

    // Allocate the array (we rebuild for each run to clear cache locality)
    std::vector<size_t> data(count);

    // Create a random pointer-chasing pattern
    std::vector<size_t> indices(count);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937(1337));

    // Create a linked list via indices
    for (size_t i = 0; i < count - 1; ++i)
        data[indices[i]] = indices[i + 1];
    data[indices.back()] = indices[0];

    // Warm-up: ensures everything is faulted into memory
    volatile size_t sink = 0;
    size_t idx = 0;
    for (size_t i = 0; i < count; ++i)
        idx = data[idx];
    sink += idx;

    for (auto _ : state) {
        // Optional: re-randomize or flush data to simulate "cold" cache
        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();

        idx = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < count; ++i)
            idx = data[idx];
        auto end = std::chrono::high_resolution_clock::now();

        // Compute nanoseconds per access
        double elapsed = std::chrono::duration<double>(end - start).count();
        double ns_per_access = (elapsed / count) * 1e9;

        state.SetIterationTime(elapsed);
        state.counters["ns_per_access"] = ns_per_access;

        sink += idx;
        benchmark::DoNotOptimize(sink);
    }
}

// Test across sizes: 1 KB to 64 MB
BENCHMARK(BM_CacheSize)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 28)
    ->UseManualTime();

BENCHMARK_MAIN();
