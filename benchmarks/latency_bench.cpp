#include <benchmark/benchmark.h>
#include "ConcurrentMinHeap.hpp"
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

double calculate_p99(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    std::vector<double> copy = v;
    std::sort(copy.begin(), copy.end());
    size_t idx = static_cast<size_t>(0.99 * copy.size());
    if (idx >= copy.size()) idx = copy.size() - 1;
    return copy[idx];
}

static void BM_HeapConcurrentPushPop(benchmark::State& state) {
    static ConcurrentMinHeap<Task> queue(500000);
    
    thread_local std::mt19937 rng(std::random_device{}());
    thread_local std::uniform_int_distribution<uint32_t> dist(1, 10000);
    
    for (auto _ : state) {
        queue.push(Task{
            .priority = dist(rng), 
            .payload_id = static_cast<uint32_t>(state.thread_index()), 
            .timestamp = 0
        });

        Task out;
        benchmark::DoNotOptimize(queue.try_pop(out));
    }
}

BENCHMARK(BM_HeapConcurrentPushPop)
    ->Threads(8)                         
    ->UseRealTime()                      
    ->Unit(benchmark::kMicrosecond)      
    ->Repetitions(10)                    // Changed from 100 to 10 for instant output
    ->DisplayAggregatesOnly(true)        
    ->ComputeStatistics("p99", calculate_p99); 

BENCHMARK_MAIN();