#include <cassert>
#include <cstddef>
#include "../include/cac/soa_container.hpp"
#include "../include/cac/aos_container.hpp"

int main() {
    cac::soa_particle_container soa{};

    for (size_t i = 0; i < N; ++i) {
        soa.x[i] += soa.vx[i];
    }
}