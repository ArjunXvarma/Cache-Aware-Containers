#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace cac::bench {

/// Statistics from a benchmark run
struct BenchmarkStats {
    double min_ns;
    double max_ns;
    double mean_ns;
    double stddev_ns;
    size_t iterations;
    size_t data_size;

    double min_ms() const { return min_ns / 1e6; }
    double max_ms() const { return max_ns / 1e6; }
    double mean_ms() const { return mean_ns / 1e6; }
    double stddev_ms() const { return stddev_ns / 1e6; }
};

/// Core benchmark harness
class BenchmarkHarness {
public:
    explicit BenchmarkHarness(size_t warmup_runs = 3, size_t measurement_runs = 10)
        : warmup_runs_(warmup_runs), measurement_runs_(measurement_runs) {}

    /// Run a benchmark and return statistics
    template<typename Fn>
    BenchmarkStats run(Fn&& fn, size_t data_size = 0) {
        // Warmup runs
        for (size_t i = 0; i < warmup_runs_; ++i) {
            fn();
        }

        // Measurement runs
        std::vector<double> durations;
        durations.reserve(measurement_runs_);

        for (size_t i = 0; i < measurement_runs_; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            fn();
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            durations.push_back(static_cast<double>(duration.count()));
        }

        return compute_stats(durations, data_size);
    }

    void set_measurement_runs(size_t runs) { measurement_runs_ = runs; }
    void set_warmup_runs(size_t runs) { warmup_runs_ = runs; }

private:
    BenchmarkStats compute_stats(const std::vector<double>& durations, size_t data_size) {
        auto min_val = *std::min_element(durations.begin(), durations.end());
        auto max_val = *std::max_element(durations.begin(), durations.end());

        double sum = 0.0;
        for (double d : durations) {
            sum += d;
        }
        double mean_val = sum / durations.size();

        double variance = 0.0;
        for (double d : durations) {
            variance += (d - mean_val) * (d - mean_val);
        }
        variance /= durations.size();
        double stddev_val = std::sqrt(variance);

        return {min_val, max_val, mean_val, stddev_val, measurement_runs_, data_size};
    }

    size_t warmup_runs_;
    size_t measurement_runs_;
};

/// Result formatting utilities
class ResultFormatter {
public:
    /// Print a benchmark result as a row
    static void print_result(const std::string& name, const BenchmarkStats& stats) {
        std::cout << std::left << std::setw(30) << name << " | "
                  << std::setw(12) << std::fixed << std::setprecision(3) << stats.min_ms() << " ms | "
                  << std::setw(12) << std::fixed << std::setprecision(3) << stats.mean_ms() << " ms | "
                  << std::setw(12) << std::fixed << std::setprecision(3) << stats.max_ms() << " ms"
                  << std::endl;
    }

    /// Print table header
    static void print_header(const std::string& title = "") {
        if (!title.empty()) {
            std::cout << "\n" << title << "\n";
            std::cout << std::string(80, '=') << "\n";
        }
        std::cout << std::left << std::setw(30) << "Benchmark" << " | "
                  << std::setw(12) << "Min (ms)" << " | "
                  << std::setw(12) << "Mean (ms)" << " | "
                  << std::setw(12) << "Max (ms)" << "\n";
        std::cout << std::string(80, '-') << "\n";
    }

    /// Print CSV header
    static void print_csv_header() {
        std::cout << "benchmark_name,size,min_ms,mean_ms,max_ms,stddev_ms\n";
    }

    /// Print result as CSV row
    static void print_csv_result(const std::string& name, const BenchmarkStats& stats) {
        std::cout << name << "," << stats.data_size << ","
                  << std::fixed << std::setprecision(6)
                  << stats.min_ms() << "," << stats.mean_ms() << ","
                  << stats.max_ms() << "," << stats.stddev_ms() << "\n";
    }

    /// Print section separator
    static void print_separator() {
        std::cout << std::string(80, '=') << "\n";
    }
};

/// RAII timer for inline timing
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed_ns() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_);
        return static_cast<double>(duration.count());
    }

    double elapsed_ms() const { return elapsed_ns() / 1e6; }
    double elapsed_us() const { return elapsed_ns() / 1e3; }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

} // namespace cac::bench
