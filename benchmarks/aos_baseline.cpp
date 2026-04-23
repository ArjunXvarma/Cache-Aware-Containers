#include <vector>
#include <chrono>
#include <iostream>
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;

int main() {
    const size_t N = 1'000'000;

    aos_container<Particle> my;
    std::vector<Particle> stl;

    auto bench = [](auto& container) {
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < 1'000'000; ++i) {
            container.push_back({1,2,3,4,5,6});
        }

        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    };

    std::cout << "Custom: " << bench(my) << "\n";
    std::cout << "std::vector: " << bench(stl) << "\n";
}