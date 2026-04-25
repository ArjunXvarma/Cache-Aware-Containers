#include <cassert>
#include "../include/cac/soa_container.hpp"
#include "../include/cac/particle.hpp"

namespace cac {

void test_soa_container() {
    soa_particle_container soa;

    soa.push_back(1,2,3,4,5,6);
    soa.push_back(7,8,9,10,11,12);

    // Range-based loop
    for (auto p : soa) {
        p.x += 1;
    }

    assert(soa[0].x == 2.f);
    assert(soa[1].x == 8.f);

    Particle p = soa[0];
    assert(p.x == 2.f);
    assert(p.vx == 4.f);

    soa[0] = Particle{100.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    assert(soa[0].x == 100.f);

    const soa_particle_container& const_soa = soa;
    auto const_proxy = const_soa[1];
    assert(const_proxy.x == 8.f);
    assert(const_proxy.vx == 10.f);
}

} // namespace cac
