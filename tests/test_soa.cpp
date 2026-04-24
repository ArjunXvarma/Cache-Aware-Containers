#include <cassert>
#include "../include/cac/soa_container.hpp"
#include "../include/cac/aos_container.hpp"

using namespace cac;

int main() {
    soa_particle_container soa;

    soa.push_back(1,2,3,4,5,6);
    soa.push_back(7,8,9,10,11,12);

    // Range-based loop
    for (auto p : soa) {
        p.x += 1;
    }

    assert(soa[0].x == 2);
    assert(soa[1].x == 8);

    // Copy semantics
    Particle p = soa[0];
    assert(p.x == 2);

    // Assignment
    soa[0] = Particle{100,0,0,0,0,0};
    assert(soa[0].x == 100);

    return 0;
}