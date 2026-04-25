#include "benchmark.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"
#include <random>

using namespace cac;
using namespace cac::bench;

namespace {

void benchmark_sequential_access() {
    ResultFormatter::print_header("Access Pattern: Sequential");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            aos.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
        }

        auto stats = harness.run(
            [&aos]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < aos.size(); ++i) {
                    acc += aos[i].x + aos[i].y + aos[i].z;
                }
                (void)acc;
            },
            size
        );

        std::string name = "Sequential " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

void benchmark_strided_access(size_t stride) {
    ResultFormatter::print_header("Access Pattern: Strided (stride=" + std::to_string(stride) + ")");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            aos.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
        }

        auto stats = harness.run(
            [&aos, stride]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < aos.size(); i += stride) {
                    acc += aos[i].x + aos[i].y + aos[i].z;
                }
                (void)acc;
            },
            size
        );

        std::string name = "Strided " + std::to_string(stride) + " - " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

void benchmark_random_access() {
    ResultFormatter::print_header("Access Pattern: Random");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        aos_container<Particle> aos;
        aos.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            aos.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
        }

        // Create random access pattern
        std::vector<size_t> indices(size);
        for (size_t i = 0; i < size; ++i) {
            indices[i] = i;
        }
        std::mt19937 rng(42);
        std::shuffle(indices.begin(), indices.end(), rng);

        auto stats = harness.run(
            [&aos, &indices]() {
                volatile float acc = 0.f;
                for (size_t idx : indices) {
                    acc += aos[idx].x + aos[idx].y + aos[idx].z;
                }
                (void)acc;
            },
            size
        );

        std::string name = "Random " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

} // namespace

void run_access_pattern_benchmarks() {
    benchmark_sequential_access();
    benchmark_strided_access(2);
    benchmark_strided_access(4);
    benchmark_strided_access(8);
    benchmark_random_access();
}
