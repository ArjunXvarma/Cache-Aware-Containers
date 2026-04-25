# Benchmark Results — Cache-Aware Containers

This document presents performance results for the cache-aware container library, focusing on **memory layout, access patterns, alignment, growth strategies, and prefetching**.

All benchmarks were run using a custom harness with:

* Warm-up iterations
* Multiple measurement runs
* Min / Mean / Max reporting

---

# Benchmark Philosophy

The goal is not just to measure speed, but to understand:

* How **data layout** affects performance
* How **access patterns** interact with CPU caches
* When low-level optimizations like **prefetching** matter

Each experiment isolates a single variable.

---

# AoS vs SoA

## Sequential Update (`x += vx`)

| Size | AoS (ms) | SoA (ms) | Improvement |
| ---- | -------- | -------- | ----------- |
| 1e5  | 0.535    | 0.516    | ~3.5%       |
| 1e6  | 5.276    | 5.098    | ~3.4%       |
| 1e7  | 52.750   | 50.849   | ~3.6%       |

### Insight

SoA provides **modest improvements (~3–4%)** for scalar sequential workloads.

* Accesses are already predictable
* Hardware prefetcher is highly effective
* Proxy abstraction introduces slight overhead

---

## Full Update (`x,y,z += vx,vy,vz`)

| Size | AoS (ms) | SoA (ms) |
| ---- | -------- | -------- |
| 1e6  | 5.211    | 5.191    |
| 1e7  | 52.281   | 51.907   |

### Insight

AoS and SoA perform similarly when **all fields are accessed**.

* AoS benefits from loading all fields in one cache line
* SoA requires multiple independent memory streams

---

# Access Patterns

## Sequential

| Size | Time (ms) |
| ---- | --------- |
| 1e6  | ~5.07     |
| 1e7  | ~50.5     |

Baseline for optimal memory access.

---

## Strided Access

| Stride | 1e7 Time (ms) |
| ------ | ------------- |
| 1      | ~50           |
| 2      | ~26           |
| 4      | ~13           |
| 8      | ~23           |

### Insight

* Performance scales roughly with **data touched**
* Plateau at higher strides due to **cache line granularity**
* Hardware prefetch becomes less effective

---

## Random Access

| Size | Time (ms) |
| ---- | --------- |
| 1e6  | ~10.2     |
| 1e7  | ~365      |

### Key Result

> Random access is ~7× slower than sequential.

### Why?

* Breaks spatial locality
* Causes frequent cache misses
* Memory latency dominates execution

---

# Alignment Impact

| Alignment | 1e7 Mean (ms) |
| --------- | ------------- |
| 32B       | ~50.7         |
| 64B       | ~50.6         |
| 128B      | ~50.4         |

### Insight

Alignment has **minimal impact** for this workload.

Reason:

* Sequential access already aligns well with cache lines
* Memory bandwidth dominates performance

---

# Growth Policy

## Push Back Performance (1e7 elements)

| Policy | Mean (ms) |
| ------ | --------- |
| Double | ~355      |
| 1.5×   | ~378      |

### Insight

* **Doubling growth is faster (~6–8%)**
* 1.5× growth causes:

  * More reallocations
  * Higher total copy cost

### Note

Max values show high variance due to:

* OS allocation behavior
* Page faults
* Memory fragmentation

Mean values are more reliable.

---

# Prefetching

## Sequential

| Size | No Prefetch | Prefetch |
| ---- | ----------- | -------- |
| 1e7  | ~50.5 ms    | ~50.5 ms |

### Insight

No improvement—hardware prefetch already optimal.

---

## Large Stride (64)

| Size | No Prefetch | Prefetch |
| ---- | ----------- | -------- |
| 1e7  | ~1.61 ms    | ~1.58 ms |

### Insight

* Slight improvement (~2–3%)
* Prefetch helps when hardware prefetch struggles

---

## Prefetch Distance Sweep

| Distance | 1e7 Mean (ms) |
| -------- | ------------- |
| 16       | ~3.52         |
| 32       | ~3.09         |
| 64       | ~2.78         |
| 128      | ~2.89         |
| 256      | ~3.04         |

### Key Insight

> There exists an optimal prefetch distance (~64).

Too small:

* Data not ready in time

Too large:

* Cache pollution

---

## Raw Array (SoA-style)

| Size | No Prefetch | Prefetch  |
| ---- | ----------- | --------- |
| 1e7  | ~50.47 ms   | ~50.45 ms |

### Insight

Even without proxy abstraction:

> Prefetching provides negligible benefit for sequential workloads.

---

# Key Takeaways

## 1. Memory Access Dominates Performance

* Sequential access is **orders of magnitude faster**
* Random access severely degrades performance

---

## 2. SoA vs AoS Depends on Access Pattern

* SoA wins for **selective field access**
* AoS performs well for **full object usage**

---

## 3. Hardware Prefetching is Highly Effective

* Manual prefetching rarely helps
* Only beneficial in:

  * large strides
  * irregular access patterns

---

## 4. Growth Strategy Matters

* Doubling reduces reallocations
* Leads to better amortized performance

---

## 5. Alignment is Context-Dependent

* No effect in sequential workloads
* More important for:

  * SIMD
  * multithreading
  * false sharing

---

# Conclusion

These results reinforce a key principle:

> **Performance is determined not just by algorithms, but by how data is accessed in memory.**

Understanding:

* cache behavior
* memory layout
* access patterns

is critical for building high-performance systems.

---

# Future Work

* SIMD/vectorization experiments
* Parallel iteration + false sharing analysis
* Irregular workloads (pointer chasing, graphs)
* NUMA-aware memory allocation
