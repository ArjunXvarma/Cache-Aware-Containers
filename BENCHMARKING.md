# Cache-Aware Container Benchmarking Suite

This document describes the comprehensive benchmarking framework for the Cache-Aware Containers library.

## Overview

The benchmarking suite measures performance characteristics of:
- **AoS (Array-of-Structures)** vs **SoA (Structure-of-Arrays)** layouts
- Different **access patterns** (sequential, strided, random)
- **Alignment strategies** (32B, 64B, 128B)
- **Growth policies** (2x vs 1.5x)
- Optional **prefetching** optimizations

## Building

```bash
cd build
cmake --build . --target cac_benchmarks
```

## Usage

### Run All Benchmarks (Default)

```bash
./benchmarks/cac_benchmarks
```

### Run Specific Benchmark Suite

```bash
./benchmarks/cac_benchmarks --aos-vs-soa          # AoS vs SoA comparison
./benchmarks/cac_benchmarks --access-patterns     # Different access patterns
./benchmarks/cac_benchmarks --alignment           # Alignment impact
./benchmarks/cac_benchmarks --growth-policy       # Growth policy comparison
./benchmarks/cac_benchmarks --prefetch            # Prefetching experiments
```

### Display Help

```bash
./benchmarks/cac_benchmarks --help
```

## Benchmark Descriptions

### 1. AoS vs SoA Benchmarks

Compares performance of Array-of-Structures and Structure-of-Arrays layouts across different workloads.

**Scenarios:**
- **Sequential Update**: Updates only X coordinate (`x += vx`)
- **Full Update**: Updates all position components (`x,y,z += vx,vy,vz`)

**Data Sizes:** 1K, 100K, 1M, 10M particles

**Why It Matters:**
- SoA typically excels with data-parallel workloads due to better cache utilization
- AoS may perform better for workloads accessing complete particles
- Results show how layout choice affects performance across different access patterns

### 2. Access Pattern Benchmarks

Evaluates performance under different data access patterns using the same container.

**Patterns Tested:**
- **Sequential**: Access elements in order (best case for prefetching)
- **Strided**: Access every N-th element (stride = 2, 4, 8)
- **Random**: Shuffled access order (worst case for cache)

**Data Sizes:** 1K, 100K, 1M, 10M elements

**Why It Matters:**
- Sequential access achieves best cache locality
- Strided access demonstrates cache line efficiency
- Random access shows worst-case behavior and DRAM bandwidth limits

### 3. Alignment Impact Benchmarks

Measures performance impact of different memory alignment strategies.

**Alignments Tested:**
- 32-byte (L1 cache line)
- 64-byte (typical L2 cache line)
- 128-byte (SIMD-friendly)

**Data Sizes:** 1K, 100K, 1M, 10M elements

**Why It Matters:**
- Proper alignment improves cache efficiency
- Larger alignments benefit SIMD operations
- Trade-off between memory usage and performance

### 4. Growth Policy Benchmarks

Compares dynamic vector growth strategies.

**Policies Tested:**
- **Double**: Capacity doubles on reallocation (2x)
- **1.5x**: Capacity increases by 50% on reallocation (1.5x)

**Metrics:** Push-back throughput across different vector sizes

**Why It Matters:**
- 2x growth minimizes reallocations but wastes memory
- 1.5x growth is more memory-efficient but causes more reallocations
- Trade-off between memory usage and performance consistency

### 5. Prefetching Benchmarks

Demonstrates the impact of explicit CPU prefetching hints.

**Scenarios:**
- **Without Prefetch**: Baseline sequential access
- **With Prefetch**: Using `__builtin_prefetch()` with lookahead distance of 8 elements

**Data Sizes:** 1K, 100K, 1M, 10M elements

**Why It Matters:**
- Prefetching can hide memory latency in certain patterns
- Effectiveness depends on workload and data size
- Demonstrates low-level performance tuning techniques

## Output Format

### Standard Table Format

```
AoS vs SoA: Sequential Update (x += vx)
================================================================================
Benchmark                      | Min (ms)     | Mean (ms)    | Max (ms)    
--------------------------------------------------------------------------------
AoS Sequential 1000            | 0.005        ms | 0.006        ms | 0.011      ms
SoA Sequential 1000            | 0.004        ms | 0.005        ms | 0.009      ms
================================================================================
```

**Columns:**
- **Benchmark**: Test name and data size
- **Min**: Minimum execution time across all runs
- **Mean**: Average execution time (primary metric)
- **Max**: Maximum execution time (shows variance)

### Interpreting Results

1. **Mean Time**: Primary metric - lower is better
2. **Min/Max Spread**: Variance indicates consistency
   - Tight range: Stable, predictable performance
   - Wide range: Contention, OS interference, or cache misses
3. **Compare Across Sizes**: Observe how performance scales
   - Flat/linear: Good cache behavior
   - Steep jump: Cache miss at a threshold

## Statistical Interpretation

Each benchmark run:
1. Performs 3 **warm-up runs** (not measured) to stabilize caches
2. Performs 10 **measurement runs** (results reported)
3. Computes min, max, mean, and standard deviation

The **mean** time is the primary metric for performance comparison.

## Performance Analysis Tips

### Finding Cache Misses

Look for **sharp increases** in execution time at specific data sizes:
- ~32 KB: L1 cache boundary (private to core)
- ~256 KB: L2 cache boundary (per-core)
- ~8 MB: L3 cache boundary (shared)
- Beyond ~50 MB: DRAM/main memory

### Comparing Layouts

When comparing AoS vs SoA:
1. **Sequential single-field access**: SoA typically wins
2. **Full particle access**: AoS may perform better
3. **Cache effects**: Look at results scaled by data size

### Alignment Impact

When comparing alignments:
1. Results should be very similar for small data sizes
2. Larger alignments show benefits for SIMD workloads
3. The "sweet spot" is usually your system's L2 cache line size

## System Information

To interpret results correctly, know your system:

```bash
# Linux/macOS
sysctl hw.cachelinesize              # Cache line size
sysctl hw.l1icachesize               # L1 cache
sysctl hw.l2cachesize                # L2 cache
sysctl hw.l3cachesize                # L3 cache (if present)

# Or use:
lscpu (Linux)
lstopo (Linux)
```

## Extending the Benchmarks

To add a new benchmark:

1. **Create a new file** in `benchmarks/` (e.g., `my_benchmark.cpp`)
2. **Implement a function** `void run_my_benchmark_benchmarks()`
3. **Update** `bench_main.cpp`:
   - Add forward declaration
   - Add CLI option
   - Call function in main
4. **Update** `CMakeLists.txt` to include the new file
5. **Rebuild**:
   ```bash
   cd build && cmake --build . --target cac_benchmarks
   ```

### Example Benchmark Template

```cpp
#include "benchmark.hpp"
#include "../include/cac/cache_vector.hpp"
#include "../include/cac/particle.hpp"

using namespace cac;
using namespace cac::bench;

namespace {

void benchmark_something() {
    ResultFormatter::print_header("My Benchmark Title");

    std::vector<size_t> sizes = {1000, 100000, 1000000, 10000000};
    BenchmarkHarness harness(3, 10);  // 3 warmup, 10 measurement runs

    for (size_t size : sizes) {
        // Setup
        cache_vector<Particle> vec;
        vec.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            vec.push_back(Particle{1.f, 2.f, 3.f, 0.1f, 0.2f, 0.3f});
        }

        // Run benchmark
        auto stats = harness.run(
            [&vec]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < vec.size(); ++i) {
                    acc += vec[i].x;
                }
                (void)acc;  // Prevent optimization
            },
            size
        );

        std::string name = "My Benchmark " + std::to_string(size);
        ResultFormatter::print_result(name, stats);
    }
    ResultFormatter::print_separator();
}

} // namespace

void run_my_benchmark_benchmarks() {
    benchmark_something();
}
```

## Performance Optimization Guidelines

Based on benchmark results, here are optimization strategies:

### For AoS Containers
- Best for **complete-particle access** workloads
- Consider when **all fields accessed together**
- Good for **SIMD operations on entire structures**

### For SoA Containers
- Best for **selective field access** workloads
- Superior **cache locality** for data-parallel operations
- Ideal for **physics simulations** where components are processed independently

### Alignment Strategies
- Use **64 bytes** as default (most systems)
- Use **32 bytes** for memory-constrained scenarios
- Use **128 bytes** for explicit SIMD workloads

### Growth Policies
- Use **2x** for predictable performance (more frequent allocations hidden)
- Use **1.5x** for memory-efficient server workloads
- Monitor reallocation frequency vs. memory waste

## Common Issues

### Benchmark Takes Too Long
- Reduce data sizes in the test files
- Reduce number of measurement runs (edit harness creation)
- Run specific benchmarks instead of all

### Inconsistent Results
- Close other applications
- Ensure CPU frequency scaling is disabled if possible
- Run multiple times and compare

### Results Don't Match Expectations
- Check system cache configuration (`lstopo`, `sysctl`)
- Verify compiler optimization flags (check `-O3` or `-O2`)
- Check if SIMD is enabled in compiler flags

## References

- [CPU Cache Behavior](https://easyperf.net/blog/)
- [Branch Prediction and Cache Optimization](https://www.anandtech.com/)
- [Performance Measurement Guidelines](https://lemire.me/)
