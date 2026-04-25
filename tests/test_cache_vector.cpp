#include <cassert>
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/aos_container.hpp"
#include "../include/cac/particle.hpp"

namespace cac {

void test_cache_vector() {
    cache_vector<Particle> v;
    assert(v.empty());

    v.reserve(1000);
    assert(v.capacity() >= 1000);

    for (int i = 0; i < 1000; ++i) {
        v.push_back({1,2,3,4,5,6});
    }

    assert(v.size() == 1000);
    assert(v.capacity() >= 1000);

    size_t count = 0;
    for (auto& p : v) {
        assert(p.x == 1.f);
        assert(p.vy == 5.f);
        ++count;
    }
    assert(count == 1000);

    v[0].x = 42.f;
    assert(v[0].x == 42.f);
}

} // namespace cac
