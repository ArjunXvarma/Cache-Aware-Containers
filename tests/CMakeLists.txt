add_executable(cac_tests
    # test_main.cpp
    # test_cache_vector.cpp
    test_aos.cpp
    # test_soa.cpp
    # test_iterators.cpp
)

target_link_libraries(cac_tests
    PRIVATE cac
)

add_test(NAME cac_tests COMMAND cac_tests)