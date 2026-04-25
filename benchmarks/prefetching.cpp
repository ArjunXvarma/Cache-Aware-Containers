#include "benchmark.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

void benchmark_without_prefetch() {
    ResultFormatter::print_header("Prefetching: Without");

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

        std::string name = "No-Prefetch " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

void benchmark_with_prefetch() {
    ResultFormatter::print_header("Prefetching: With __builtin_prefetch");

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
                const size_t prefetch_distance = 8;
                for (size_t i = 0; i < aos.size(); ++i) {
                    if (i + prefetch_distance < aos.size()) {
                        __builtin_prefetch(&aos[i + prefetch_distance], 0, 3);
                    }
                    acc += aos[i].x + aos[i].y + aos[i].z;
                }
                (void)acc;
            },
            size
        );

        std::string name = "With-Prefetch " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

} // namespace

void run_prefetch_benchmarks() {
    benchmark_without_prefetch();
    benchmark_with_prefetch();
}
