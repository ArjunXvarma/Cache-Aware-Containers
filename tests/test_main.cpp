#include <iostream>

namespace cac {
    void test_aos_container();
    void test_soa_container();
    void test_cache_vector();
    void test_growth_policy();
}

int main() {
    cac::test_aos_container();
    cac::test_soa_container();
    cac::test_cache_vector();
    cac::test_growth_policy();

    std::cout << "All tests passed.\n";
    return 0;
}
