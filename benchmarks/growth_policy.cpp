#include "benchmark.hpp"
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

template<GrowthPolicy GP>
void benchmark_growth_policy_variant(const std::string& policy_name) {
    ResultFormatter::print_header("Growth Policy: " + policy_name);

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        cache_vector<Particle, 64, GP> vec;

        auto stats = harness.run(
            [&vec, size]() {
                for (size_t i = 0; i < size; ++i) {
                    vec.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
                }
            },
            size
        );

        std::string name = policy_name + " push_back " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

} // namespace

void run_growth_policy_benchmarks() {
    benchmark_growth_policy_variant<GrowthPolicy::Double>("Double");
    benchmark_growth_policy_variant<GrowthPolicy::OnePointFive>("1.5x");
}
