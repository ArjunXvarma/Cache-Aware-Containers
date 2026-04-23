#include <cassert>
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;

int main() {
    aos_container<Particle> particles;

    particles.push_back({1,2,3,4,5,6});
    particles.push_back({7,8,9,10,11,12});

    assert(particles.size() == 2);

    assert(particles[0].x == 1);
    assert(particles[1].vx == 10);

    // Iterator test
    float sum = 0;
    for (auto& p : particles) {
        sum += p.x;
    }

    assert(sum == 8);

    return 0;
}