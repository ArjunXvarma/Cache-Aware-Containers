#include <cassert>
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;

int main() {
    cache_vector<Particle> v;

    for (int i = 0; i < 1000; ++i) {
        v.push_back({1,2,3,4,5,6});
    }

    assert(v.size() == 1000);

    for (auto& p : v) {
        assert(p.x == 1);
    }

    return 0;
}