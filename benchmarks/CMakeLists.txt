add_executable(cac_benchmarks
    bench_main.cpp
    aos_vs_soa.cpp
    access_patterns.cpp
    alignment.cpp
    growth_policy.cpp
    prefetching.cpp
)

target_link_libraries(cac_benchmarks
    PRIVATE cac
)

# Optional: link pthread for multithread tests
find_package(Threads REQUIRED)
target_link_libraries(cac_benchmarks PRIVATE Threads::Threads)