# Cache-Aware Containers in Modern C++

A high-performance, data-oriented container library exploring **memory layout, cache efficiency, and abstraction design** in modern C++.

This project implements **Array-of-Structs (AoS)** and **Struct-of-Arrays (SoA)** containers with STL-like interfaces, alongside a custom **cache-aware vector**, and evaluates their behavior under different access patterns and memory conditions.

---

## Motivation

Modern CPUs are **not compute-bound—they are memory-bound**.

The performance of many systems (games, simulations, trading systems, compilers) is dictated not by algorithmic complexity, but by:

* Cache locality
* Memory access patterns
* Data layout

This project explores a fundamental question:

> *How does data layout impact performance, and how can we design abstractions that preserve both usability and efficiency?*

---

## Core Components

### 1. AoS Container (`aos_container<T>`)

A traditional contiguous container:

```cpp
struct Particle {
    float x, y, z;
    float vx, vy, vz;
};
```

Memory layout:

```
[x y z vx vy vz][x y z vx vy vz]...
```

**Characteristics:**

* Strong spatial locality for full-object access
* Simple iterator model (`T*`)
* Matches `std::vector<T>` semantics

---

### 2. SoA Container (`soa_particle_container`)

A data-oriented layout:

```
x:  [x x x x x]
y:  [y y y y y]
z:  [z z z z z]
vx: [v v v v v]
...
```

**Key idea:**

> Separate data by field to improve cache efficiency for selective access.

---

### 3. Proxy-Based Abstraction

SoA does not store actual `Particle` objects.

To preserve usability, the container returns a **proxy reference**:

```cpp
for (auto& p : particles) {
    p.x += p.vx;
}
```

Under the hood:

```cpp
struct ParticleProxy {
    float& x, y, z;
    float& vx, vy, vz;
};
```

---

### Why this matters

This enables:

* STL-like iteration
* Field-level memory control
* Clean separation of **logical view vs physical layout**

This pattern is used in:

* Eigen
* game engines (ECS systems)
* high-performance numerical libraries

---

### 4. Cache-Aware Vector (`cache_vector<T>`)

A custom vector implementation with:

* **Aligned allocation** (default: 64 bytes)
* Configurable **growth strategies**
* Manual memory management
* Exception-safe reallocation

```cpp
cache_vector<Particle, 64, GrowthPolicy::Double>
```

---

## Design Deep Dive

### 1. Memory Layout vs Performance

The central tradeoff:

| Layout | Strength                       | Weakness                  |
| ------ | ------------------------------ | ------------------------- |
| AoS    | Good for full-object access    | Poor for selective fields |
| SoA    | Excellent for selective access | Multiple memory streams   |

---

### 2. Cache Behavior

Modern CPUs fetch data in **cache lines (~64 bytes)**.

#### AoS access:

```cpp
p.x += p.vx;
```

Loads:

```
[x y z vx vy vz]
```

**Wasted bandwidth** if only `x` and `vx` are needed.

---

#### SoA access:

```cpp
x[i] += vx[i];
```

Loads:

```
[x x x x x] and [v v v v v]
```

**Perfect spatial locality**

---

### 3. Proxy vs Real References

AoS:

```cpp
Particle& operator*();
```

SoA:

```cpp
ParticleProxy operator*();
```

This introduces a tradeoff:

* Enables abstraction
* Adds minor overhead
* Can limit compiler optimizations

---

### 4. Alignment Strategy

Memory is aligned to cache-line boundaries:

```cpp
Alignment = 64 bytes
```

Goals:

* Avoid split cache lines
* Enable SIMD-friendly access
* Improve predictability

---

### 5. Growth Policy Tradeoffs

Two strategies:

* **2× growth** → fewer reallocations
* **1.5× growth** → lower memory overhead

Tradeoff:

| Strategy | Pros             | Cons                |
| -------- | ---------------- | ------------------- |
| 2×       | Faster inserts   | Higher memory usage |
| 1.5×     | Memory efficient | More reallocations  |

---

### 6. Prefetching Reality Check

Manual prefetching was evaluated using:

```cpp
__builtin_prefetch(...)
```

Key finding:

> Modern CPUs already implement highly effective hardware prefetchers.

<!-- Result:

* Minimal to no improvement for sequential workloads
* Highlights limits of manual optimization -->

---

## Benchmark Philosophy

Benchmarks are designed to isolate:

* Memory layout effects
* Access patterns (sequential, strided, random)
* Growth behavior
* Alignment sensitivity

**Important principle:**

> Measure one variable at a time.

Detailed results are available in:

```
benchmarks/README.md
```

---

## Key Insights

* **Memory access dominates performance** for large datasets
* **Sequential access is orders of magnitude faster than random access**
* **SoA improves performance for selective field access**
* **AoS performs well when full objects are used**
* **Hardware prefetching is highly effective**
* **Abstraction layers (proxies) introduce measurable but small overhead**

---

## Project Structure

```
include/cac/
  aos_container.hpp
  soa_container.hpp
  cache_vector.hpp
  allocator.hpp
  iterator.hpp
  proxy.hpp

benchmarks/
  benchmark.hpp
  *.cpp
  README.md   # detailed results

tests/
  *.cpp

examples/
  *.cpp
```

---

## Building

```bash
mkdir build
cd build
cmake ..
make
```

Run benchmarks:

```bash
./benchmarks/cac_benchmarks
```

---

## What This Project Demonstrates

* Deep understanding of:

  * CPU cache hierarchy
  * memory layout design
  * data-oriented programming

* Ability to:

  * build STL-like abstractions
  * reason about performance tradeoffs
  * design meaningful benchmarks

---

## Future Work

* SIMD vectorization experiments
* Parallel iteration + false sharing analysis
* Generalized SoA container (template-based fields)
* Integration with `std::ranges`

---

## Takeaway

> Performance is not just about algorithms—it’s about how data moves through memory.

This project explores how **data layout decisions directly impact real-world performance**, and how to design abstractions that respect both **usability and hardware constraints**.
