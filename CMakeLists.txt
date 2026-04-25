cmake_minimum_required(VERSION 3.20)

project(CacheAwareContainers
    VERSION 1.0
    DESCRIPTION "Cache-aware container library (AoS vs SoA, aligned storage, benchmarking)"
    LANGUAGES CXX
)

# -------------------------------
# Global Settings
# -------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands (for clangd, tooling)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# -------------------------------
# Options
# -------------------------------
option(CAC_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(CAC_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(CAC_BUILD_TESTS "Build unit tests" OFF)
option(CAC_BUILD_BENCHMARKS "Build benchmarks" OFF)

# -------------------------------
# Include custom CMake modules
# -------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(StandardSettings OPTIONAL)
include(CompilerWarnings OPTIONAL)
include(Sanitizers OPTIONAL)

# -------------------------------
# Library Target
# -------------------------------
add_library(cac INTERFACE)

target_include_directories(cac
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(cac INTERFACE cxx_std_20)

# High-performance flags
target_compile_options(cac INTERFACE
    -O3
    -march=native
)

# -------------------------------
# Warnings (optional but strong signal)
# -------------------------------
target_compile_options(cac INTERFACE -fsanitize=address -g)
target_link_options(cac INTERFACE -fsanitize=address)
if(CAC_ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(cac INTERFACE
            -Wall -Wextra -Wpedantic
            -Wshadow -Wconversion
            -Wnull-dereference
            -Wold-style-cast
        )
    endif()
endif()

# -------------------------------
# Subdirectories
# -------------------------------
if(CAC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CAC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()