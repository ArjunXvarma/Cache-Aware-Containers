#include "benchmark.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/soa_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

void benchmark_aos_vs_soa_sequential_update() {
    ResultFormatter::print_header("AoS vs SoA: Sequential Update (x += vx)");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        // AoS benchmark
        {
            aos_container<Particle> aos;
            aos.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                aos.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
            }

            auto stats = harness.run(
                [&aos]() {
                    volatile float acc = 0.f;
                    for (size_t i = 0; i < aos.size(); ++i) {
                        aos[i].x += aos[i].vx;
                        acc += aos[i].x;
                    }
                    // Prevent optimization
                    (void)acc;
                },
                size
            );

            std::string name = "AoS Sequential " + std::to_string(size);
            ResultFormatter::print_result(name, stats);
        }

        // SoA benchmark
        {
            soa_particle_container soa;
            for (size_t i = 0; i < size; ++i) {
                soa.push_back(1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f);
            }

            auto stats = harness.run(
                [&soa]() {
                    volatile float acc = 0.f;
                    for (size_t i = 0; i < soa.size(); ++i) {
                        auto p = soa[i];
                        p.x += p.vx;
                        acc += p.x;
                    }
                    (void)acc;
                },
                size
            );

            std::string name = "SoA Sequential " + std::to_string(size);
            ResultFormatter::print_result(name, stats);
        }

        ResultFormatter::print_separator();
    }
}

void benchmark_aos_vs_soa_full_update() {
    ResultFormatter::print_header("AoS vs SoA: Full Update (x,y,z += vx,vy,vz)");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);

    for (size_t size : sizes) {
        // AoS benchmark
        {
            aos_container<Particle> aos;
            aos.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                aos.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
            }

            auto stats = harness.run(
                [&aos]() {
                    volatile float acc = 0.f;
                    for (size_t i = 0; i < aos.size(); ++i) {
                        aos[i].x += aos[i].vx;
                        aos[i].y += aos[i].vy;
                        aos[i].z += aos[i].vz;
                        acc += aos[i].x + aos[i].y + aos[i].z;
                    }
                    (void)acc;
                },
                size
            );

            std::string name = "AoS Full " + std::to_string(size);
            ResultFormatter::print_result(name, stats);
        }

        // SoA benchmark
        {
            soa_particle_container soa;
            for (size_t i = 0; i < size; ++i) {
                soa.push_back(1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f);
            }

            auto stats = harness.run(
                [&soa]() {
                    volatile float acc = 0.f;
                    for (size_t i = 0; i < soa.size(); ++i) {
                        auto p = soa[i];
                        p.x += p.vx;
                        p.y += p.vy;
                        p.z += p.vz;
                        acc += p.x + p.y + p.z;
                    }
                    (void)acc;
                },
                size
            );

            std::string name = "SoA Full " + std::to_string(size);
            ResultFormatter::print_result(name, stats);
        }

        ResultFormatter::print_separator();
    }
}

} // namespace

void run_aos_vs_soa_benchmarks() {
    benchmark_aos_vs_soa_sequential_update();
    benchmark_aos_vs_soa_full_update();
}