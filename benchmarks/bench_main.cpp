#include <vector>
#include <chrono>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

// Forward declarations for all benchmarks
void run_aos_vs_soa_benchmarks();
void run_access_pattern_benchmarks();
void run_alignment_benchmarks();
void run_growth_policy_benchmarks();
void run_prefetch_benchmarks();

struct BenchmarkConfig {
    std::string benchmark_name;
    bool run_all = false;
    bool csv_output = false;
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n"
              << "\nOptions:\n"
              << "  --all              Run all benchmarks (default)\n"
              << "  --aos-vs-soa       Run AoS vs SoA benchmarks\n"
              << "  --access-patterns  Run access pattern benchmarks\n"
              << "  --alignment        Run alignment benchmarks\n"
              << "  --growth-policy    Run growth policy benchmarks\n"
              << "  --prefetch         Run prefetching benchmarks\n"
              << "  --csv              Output results in CSV format\n"
              << "  --help             Show this help message\n";
}

BenchmarkConfig parse_args(int argc, char* argv[]) {
    BenchmarkConfig config;
    config.run_all = true;  // Default: run all

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--all") {
            config.run_all = true;
            config.benchmark_name.clear();
        } else if (arg == "--aos-vs-soa") {
            config.run_all = false;
            config.benchmark_name = "aos_vs_soa";
        } else if (arg == "--access-patterns") {
            config.run_all = false;
            config.benchmark_name = "access_patterns";
        } else if (arg == "--alignment") {
            config.run_all = false;
            config.benchmark_name = "alignment";
        } else if (arg == "--growth-policy") {
            config.run_all = false;
            config.benchmark_name = "growth_policy";
        } else if (arg == "--prefetch") {
            config.run_all = false;
            config.benchmark_name = "prefetch";
        } else if (arg == "--csv") {
            config.csv_output = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            std::exit(1);
        }
    }

    return config;
}

int main(int argc, char* argv[]) {
    BenchmarkConfig config = parse_args(argc, argv);

    std::cout << "================================================================================\n";
    std::cout << "        Cache-Aware Container Benchmarking Suite\n";
    std::cout << "================================================================================\n\n";

    if (config.run_all) {
        std::cout << "Running all benchmarks...\n\n";
        run_aos_vs_soa_benchmarks();
        run_access_pattern_benchmarks();
        run_alignment_benchmarks();
        run_growth_policy_benchmarks();
        run_prefetch_benchmarks();
    } else if (config.benchmark_name == "aos_vs_soa") {
        run_aos_vs_soa_benchmarks();
    } else if (config.benchmark_name == "access_patterns") {
        run_access_pattern_benchmarks();
    } else if (config.benchmark_name == "alignment") {
        run_alignment_benchmarks();
    } else if (config.benchmark_name == "growth_policy") {
        run_growth_policy_benchmarks();
    } else if (config.benchmark_name == "prefetch") {
        run_prefetch_benchmarks();
    }

    std::cout << "\n================================================================================\n";
    std::cout << "        Benchmarking Complete\n";
    std::cout << "================================================================================\n";

    return 0;
}