#include <vector>
#include <chrono>
#include <iostream>
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;

int main() {
    const size_t N = 5'000'000;

    auto bench = [&](auto& container) {
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < N; ++i) {
            container.push_back({1,2,3,4,5,6});
        }

        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    };

    cache_vector<Particle> custom;
    std::vector<Particle> stl;

    std::cout << "Custom: " << bench(custom) << "\n";
    std::cout << "std::vector: " << bench(stl) << "\n";
}