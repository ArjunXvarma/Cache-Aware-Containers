#include "benchmark.hpp"
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

template<size_t Align>
void benchmark_alignment_variant() {
    ResultFormatter::print_header("Alignment Impact: " + std::to_string(Align) + " Bytes");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        cache_vector<Particle, Align> vec;
        vec.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            vec.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
        }

        auto stats = harness.run(
            [&vec]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < vec.size(); ++i) {
                    acc += vec[i].x + vec[i].y + vec[i].z;
                }
                (void)acc;
            },
            size
        );

        std::string name = "Align-" + std::to_string(Align) + " " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

} // namespace

void run_alignment_benchmarks() {
    benchmark_alignment_variant<32>();
    benchmark_alignment_variant<64>();
    benchmark_alignment_variant<128>();
}
