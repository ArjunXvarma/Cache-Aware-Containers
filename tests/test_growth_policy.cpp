#include <cassert>
#include "../include/cac/cache_vector.hpp"

#include <iostream>

namespace cac {

void test_growth_policy() {
    static_assert(GrowthPolicy::Double == GrowthPolicy::Double, "GrowthPolicy should compile");

    assert(next_capacity(0, GrowthPolicy::Double) == 1);
    assert(next_capacity(0, GrowthPolicy::OnePointFive) == 1);

    assert(next_capacity(1, GrowthPolicy::Double) == 2);
    assert(next_capacity(1, GrowthPolicy::OnePointFive) == 2);
    assert(next_capacity(2, GrowthPolicy::OnePointFive) == 3);
    assert(next_capacity(3, GrowthPolicy::OnePointFive) == 4);
    assert(next_capacity(7, GrowthPolicy::OnePointFive) == 10);

    cache_vector<int, 64, GrowthPolicy::Double> double_vec;
    for (int i = 0; i < 10; ++i) {
        double_vec.push_back(i);
    }
    assert(double_vec.size() == 10);
    assert(double_vec.capacity() >= 10);

    cache_vector<int, 64, GrowthPolicy::OnePointFive> one_five_vec;
    for (int i = 0; i < 10; ++i) {
        one_five_vec.push_back(i);
    }
    assert(one_five_vec.size() == 10);
    assert(one_five_vec.capacity() >= 10);
}

} // namespace cac
