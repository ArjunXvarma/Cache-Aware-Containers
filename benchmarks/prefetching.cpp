#include "benchmark.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

void benchmark_sequential_baseline() {
    ResultFormatter::print_header("Prefetching: Sequential Baseline");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            aos.push_back({1.f,2.f,3.f,0.1f,0.2f,0.3f});
        }

        auto stats = harness.run([&]() {
            volatile float acc = 0.f;
            for (size_t i = 0; i < aos.size(); ++i) {
                acc += aos[i].x;
            }
            (void)acc;
        }, size);

        ResultFormatter::print_result("Seq No-Prefetch " + std::to_string(size), stats);
    }

    ResultFormatter::print_separator();
}

void benchmark_large_stride_prefetch() {
    ResultFormatter::print_header("Prefetching: Large Stride (64)");

    std::vector<size_t> sizes = {100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    constexpr size_t stride = 64;
    constexpr size_t prefetch_dist = 256;

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            aos.push_back({1.f,2.f,3.f,0.1f,0.2f,0.3f});
        }

        // No prefetch
        auto no_prefetch = harness.run([&]() {
            volatile float acc = 0.f;
            for (size_t i = 0; i < aos.size(); i += stride) {
                acc += aos[i].x;
            }
            (void)acc;
        }, size);

        ResultFormatter::print_result("Stride64 NoPref " + std::to_string(size), no_prefetch);

        // With prefetch
        auto with_prefetch = harness.run([&]() {
            volatile float acc = 0.f;
            for (size_t i = 0; i < aos.size(); i += stride) {
                if (i + prefetch_dist < aos.size()) {
                    __builtin_prefetch(&aos[i + prefetch_dist], 0, 1);
                }
                acc += aos[i].x;
            }
            (void)acc;
        }, size);

        ResultFormatter::print_result("Stride64 Pref " + std::to_string(size), with_prefetch);
    }

    ResultFormatter::print_separator();
}

void benchmark_prefetch_distance() {
    ResultFormatter::print_header("Prefetch Distance Sweep");

    std::vector<size_t> sizes = {1000000, 10000000};
    std::vector<size_t> distances = {16, 32, 64, 128, 256};

    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            aos.push_back({1.f,2.f,3.f,0.1f,0.2f,0.3f});
        }

        for (size_t dist : distances) {
            auto stats = harness.run([&]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < aos.size(); i += 32) {
                    if (i + dist < aos.size()) {
                        __builtin_prefetch(&aos[i + dist], 0, 1);
                    }
                    acc += aos[i].x;
                }
                (void)acc;
            }, size);

            ResultFormatter::print_result(
                "Dist" + std::to_string(dist) + " " + std::to_string(size),
                stats
            );
        }
    }

    ResultFormatter::print_separator();
}

void benchmark_raw_array_prefetch() {
    ResultFormatter::print_header("Prefetching: Raw Array (SoA-style)");

    std::vector<size_t> sizes = {100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        std::vector<float> x(size, 1.f);
        std::vector<float> vx(size, 0.1f);

        // No prefetch
        auto no_prefetch = harness.run([&]() {
            volatile float acc = 0.f;
            for (size_t i = 0; i < size; ++i) {
                acc += x[i];
            }
            (void)acc;
        }, size);

        ResultFormatter::print_result("Raw NoPref " + std::to_string(size), no_prefetch);

        // With prefetch
        auto with_prefetch = harness.run([&]() {
            volatile float acc = 0.f;
            for (size_t i = 0; i < size; ++i) {
                if (i + 64 < size) {
                    __builtin_prefetch(&x[i + 64], 0, 1);
                }
                acc += x[i];
            }
            (void)acc;
        }, size);

        ResultFormatter::print_result("Raw Pref " + std::to_string(size), with_prefetch);
    }

    ResultFormatter::print_separator();
}

} // namespace

void run_prefetch_benchmarks() {
    benchmark_sequential_baseline();
    benchmark_large_stride_prefetch();
    benchmark_prefetch_distance();  
    benchmark_raw_array_prefetch();
}