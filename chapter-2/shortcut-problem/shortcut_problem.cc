#include <benchmark/benchmark.h>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

// Forward declare
void step(float *r, const float *d, int n);

#ifdef V0
void step(float *r, const float *d, int n) {
  // asm("# execution starts here");
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      float v = std::numeric_limits<float>::infinity();
      // asm("# loop start");
      for (int k = 0; k < n; ++k) {
        float x = d[n * i + k];
        float y = d[n * k + j];
        float z = x + y;
        v = std::min(v, z);
      }
      // asm("# loop end");
      r[n * i + j] = v;
    }
  }
  // asm("# execution ends here");
}
#endif

#ifdef OPENMP
void step(float *r, const float *d, int n) {
#pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      float v = std::numeric_limits<float>::infinity();
      for (int k = 0; k < n; ++k) {
        float x = d[n * i + k];
        float y = d[n * k + j];
        float z = x + y;
        v = std::min(v, z);
      }
      r[n * i + j] = v;
    }
  }
}
#endif

std::vector<float> randomize(int n) {
  std::vector<float> ret(n, 0.0f);
  std::mt19937 rng(1337);
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  for (float &data : ret)
    data = dist(rng);
  return ret;
}

static void BM_Step(benchmark::State &state) {
  const int n = state.range(0);
  const int total = n * n;

  const auto d = randomize(total);
  std::vector<float> r(total, 0.0f);

  for (auto _ : state) {
    step(r.data(), d.data(), n);
    benchmark::ClobberMemory();
  }

  // Set FLOPs per iteration: 2 * n^3
  // 1 add, 1 min() = 2 ops, assume min is 1 single floating point ops.
  state.counters["FLOPs"] = benchmark::Counter(
      2.0 * n * n * n, benchmark::Counter::kIsIterationInvariantRate);
}

BENCHMARK(BM_Step)->DenseRange(100, 7000, 500)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

// int main() {
//     constexpr int n = 4000;
//     const auto d = randomize(n*n);
//     std::vector<float> r(n*n, 0);

//     auto t0 = std::chrono::steady_clock::now();
//     step(r.data(), d.data(), n);
//     auto t1 = std::chrono::steady_clock::now();
//     std::chrono::duration<double> elapsed = t1 - t0;

//     // calculate mean to actually read the data.
//     float mean = 0.0f;
//     for (int i = 0; i < n; ++i) {
//         for (int j = 0; j < n; ++j) {
//             mean += r[i*n + j] / (n*n);
//             // std::cout << r[i*n + j] << " ";
//         }
//         // std::cout << "\n";
//     }
//     std::cout << "Calculation complete, mean: " << mean << '\n';
//     std::cout << "Wall clock: " << elapsed.count() << "s\n";
// }
