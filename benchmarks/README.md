# Cache-Aware Containers - Benchmarking Suite

A **professional-grade benchmarking framework** for evaluating performance characteristics of cache-aware container implementations.

## 🎯 What You Get

### Five Focused Benchmark Suites

1. **AoS vs SoA Comparison** (`--aos-vs-soa`)
   - Sequential single-field updates
   - Full multi-field updates
   - Shows layout efficiency across workloads

2. **Access Pattern Analysis** (`--access-patterns`)
   - Sequential (baseline)
   - Strided (varying stride: 2, 4, 8)
   - Random (worst-case)
   - Reveals cache behavior

3. **Alignment Impact** (`--alignment`)
   - 32-byte alignment
   - 64-byte alignment
   - 128-byte alignment
   - Measures cache-line efficiency

4. **Growth Policy Comparison** (`--growth-policy`)
   - 2x growth (fewer reallocations)
   - 1.5x growth (memory-efficient)
   - Evaluates trade-offs

5. **Prefetching Demonstration** (`--prefetch`)
   - Baseline sequential access
   - With `__builtin_prefetch()` lookahead
   - Shows optimization impact

### Rigorous Measurement Framework

- ✅ **3 warmup runs** to stabilize caches
- ✅ **10 measurement runs** for confidence
- ✅ **Statistical analysis**: min, max, mean, stddev
- ✅ **High-resolution timing**: nanosecond precision
- ✅ **Optimization prevention**: volatile accumulators
- ✅ **Data scaling**: 1K → 10M elements (reveals cache effects)

### Professional Output

```
================================================================================
AoS vs SoA: Sequential Update (x += vx)
================================================================================
Benchmark                      | Min (ms)     | Mean (ms)    | Max (ms)    
--------------------------------------------------------------------------------
AoS Sequential 1000            | 0.005        ms | 0.006        ms | 0.011      ms
SoA Sequential 1000            | 0.004        ms | 0.005        ms | 0.009      ms
...
```

### Clean CLI Interface

```bash
./cac_benchmarks --aos-vs-soa      # Run AoS vs SoA benchmarks
./cac_benchmarks --alignment        # Test alignment impact
./cac_benchmarks --access-patterns  # Evaluate access patterns
./cac_benchmarks --growth-policy    # Compare growth strategies
./cac_benchmarks --prefetch         # Test prefetching
./cac_benchmarks                    # Run all benchmarks (default)
./cac_benchmarks --help             # Show help
```

## 🚀 Quick Start

### Build

```bash
cd build
cmake --build . --target cac_benchmarks
```

### Run

```bash
# Run all benchmarks
./benchmarks/cac_benchmarks

# Run specific benchmark
./benchmarks/cac_benchmarks --aos-vs-soa

# Get help
./benchmarks/cac_benchmarks --help
```

## 📊 Interpreting Results

### Key Metrics

| Metric | Meaning |
|--------|---------|
| **Min** | Best-case performance (tightest loops) |
| **Mean** | Average performance (primary metric) |
| **Max** | Worst-case performance (cache misses) |
| **Spread** | Consistency (tight = stable, wide = noisy) |

### Reading Performance Jumps

Performance changes reveal cache boundaries:

- **~32 KB**: L1 cache boundary
- **~256 KB**: L2 cache boundary  
- **~8 MB**: L3 cache boundary
- **Beyond 50 MB**: DRAM bandwidth limited

### Layout Comparison

**AoS (Array-of-Structures) Better For:**
- Complete particle access workloads
- When all fields needed together
- Simpler memory management

**SoA (Structure-of-Arrays) Better For:**
- Selective field access
- Data-parallel operations
- Better cache locality for components
- Physics simulations

## 📁 File Structure

```
benchmarks/
├── benchmark.hpp                    # Core framework (200 lines)
├── bench_main.cpp                   # CLI entry point (110 lines)
├── aos_vs_soa.cpp                   # AoS vs SoA benchmarks (120 lines)
├── access_patterns.cpp              # Access pattern tests (140 lines)
├── alignment.cpp                    # Alignment tests (45 lines)
├── growth_policy.cpp                # Growth policy tests (45 lines)
└── prefetching.cpp                  # Prefetch tests (85 lines)

├── CMakeLists.txt                   # Build configuration
└── ../BENCHMARKING.md               # Comprehensive user guide
```

## 🏗️ Architecture

### Core Components

**`BenchmarkHarness`**
- Runs benchmark with configurable warmup/measurement runs
- Handles timing and statistical computation
- Prevents compiler optimization of benchmark work

**`ResultFormatter`**
- Formats results as human-readable tables
- Supports CSV output for data analysis
- Clean, aligned column output

**Benchmark Functions**
- Each suite is independent
- Easy to add new benchmarks
- Reusable components

### Design Principles

✅ **Modular**: Independent benchmark suites
✅ **Reusable**: Composable framework components
✅ **Clean**: Minimal abstractions, readable code
✅ **Fast**: Pre-allocation, minimal overhead
✅ **Rigorous**: Warm-ups, multiple runs, statistics
✅ **Extensible**: Simple template for new benchmarks
✅ **Professional**: Quality output, comprehensive docs

## 🔧 Customization

### Configure Runs

Edit benchmark files to change:
- Warmup runs: `BenchmarkHarness harness(3, 10)` → `(5, 20)`
- Data sizes: `{1000, 100000, 1000000, 10000000}` → custom list
- Prefetch distance: `const size_t prefetch_distance = 8`

### Add New Benchmark

1. Create `benchmarks/my_benchmark.cpp`:

```cpp
#include "benchmark.hpp"
#include "../include/cac/cache_vector.hpp"

using namespace cac;
using namespace cac::bench;

namespace {
    void benchmark_my_test() {
        ResultFormatter::print_header("My Test");
        BenchmarkHarness harness(3, 10);
        
        for (size_t size : {1000, 100000, 1000000, 10000000}) {
            cache_vector<Particle> vec;
            vec.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                vec.push_back(Particle{1,2,3,0.1f,0.2f,0.3f});
            }
            
            auto stats = harness.run([&vec]() {
                volatile float acc = 0.f;
                for (size_t i = 0; i < vec.size(); ++i) {
                    acc += vec[i].x;
                }
                (void)acc;
            }, size);
            
            ResultFormatter::print_result("Test " + std::to_string(size), stats);
        }
        ResultFormatter::print_separator();
    }
}

void run_my_benchmark_benchmarks() {
    benchmark_my_test();
}
```

2. Update `bench_main.cpp`:
   - Add forward declaration
   - Add CLI option
   - Call function in main

3. Update `CMakeLists.txt`:
   - Add `my_benchmark.cpp` to sources

4. Rebuild:
```bash
cd build && cmake --build . --target cac_benchmarks
```

## 📚 Documentation

- **`BENCHMARKING.md`**: Comprehensive user guide
  - Detailed benchmark descriptions
  - Performance analysis tips
  - System-specific interpretation guidance
  - Optimization guidelines
  
- **`BENCHMARKING_IMPLEMENTATION.md`**: Technical overview
  - Component descriptions
  - Design decisions
  - Code quality notes
  - Future enhancement ideas

## ⚡ Performance Tips

### For Reproducibility

1. Close other applications
2. Run benchmarks multiple times
3. Compare mean times (min is too optimistic)
4. Check for wide min/max spread (indicates noise)

### For Accuracy

1. Warm-up runs (already done)
2. Multiple measurement runs (default: 10)
3. Pre-allocate all data (avoid measuring allocation)
4. Use volatile to prevent optimizations

### For Analysis

1. Plot mean times vs. data size
2. Look for sharp jumps (cache boundaries)
3. Compare across different alignments
4. Correlate with system cache sizes

## 🔬 What's Measured

### Instruction-Level
- CPU cycles
- Cache hits/misses (indirectly through timing)
- Memory bandwidth utilization

### Algorithm-Level
- Layout efficiency (AoS vs SoA)
- Access pattern impact (sequential vs. random)
- Alignment effectiveness

### System-Level
- Cache hierarchy behavior
- DRAM bandwidth
- Memory allocation overhead

## 🎓 Learning Outcomes

After running these benchmarks, you'll understand:

1. ✅ When AoS or SoA is better for your workload
2. ✅ How alignment affects cache performance
3. ✅ Impact of access patterns on performance
4. ✅ Trade-offs in memory allocation strategies
5. ✅ Value of low-level optimizations (prefetching)
6. ✅ How to measure performance rigorously

## ⚙️ Requirements

- **C++20** compiler (g++, clang, MSVC)
- **CMake 3.12+**
- Standard library only (no external dependencies)
- macOS, Linux, or Windows

## 📝 License

Same as Cache-Aware Containers project

## 🤝 Contributing

To add benchmarks:
1. Follow the template structure
2. Use the `BenchmarkHarness` framework
3. Include comprehensive documentation
4. Test thoroughly
5. Update CMakeLists.txt and main entry point

## ❓ FAQ

**Q: Why 3 warmup runs?**
A: Sufficient to stabilize caches without excessive overhead. Adjust if needed.

**Q: Why 10 measurement runs?**
A: Balances confidence (tighter stats) with runtime. Min/max spread shows reliability.

**Q: Can I change data sizes?**
A: Yes! Edit the `sizes` vector in any benchmark function.

**Q: How do I know if results are valid?**
A: Tight min/max spread indicates stable, reproducible results. Wide spread suggests OS interference.

**Q: Should I run on isolated CPU?**
A: Not necessary, but helps reduce variance. Default results are reproducible.

---

**Happy Benchmarking!** 🚀
