# High-Performance Resource Allocator

A concurrent, lock-optimized Min-Heap architecture written in C++20, designed to optimize real-time server task and workload distribution. The system achieves a deterministic p99 latency of **< 0.12 microseconds** (116 ns) under heavy multi-threaded contention.

## Architecture & Optimizations

Standard linear scheduling algorithms operate in $O(n)$ time and bottleneck entirely under concurrent bursts. This system reduces allocation time complexity to $O(\log n)$ while guaranteeing deterministic latency via strict memory hygiene:

*   **Cache-Line Alignment:** Data structures (`Task`) and lock states are explicitly padded and aligned to 64-byte boundaries (`alignas(64)`) to perfectly match L1 cache lines, eliminating false sharing across threads.
*   **Zero-Allocation Critical Sections:** Underlying heap memory is pre-reserved to prevent $O(n)$ OS-level vector reallocation spikes inside the mutex lock.
*   **C++20 Concepts & Spaceship Operator:** Utilizes `<concepts>` (`std::totally_ordered`) and default three-way comparison operators (`<=>`) for strict, optimized weak ordering of task priorities.
*   **Branch Prediction:** Implements `[[likely]]` and `[[unlikely]]` attributes to optimize CPU instruction pipelines for the standard high-throughput path.

## Performance Validation

System throughput and tail latency were profiled using Google Benchmark. Under heavy 8-thread contention, the system reliably processes concurrent requests with sub-microsecond determinism.

| Benchmark | Time | CPU | Iterations |
| :--- | :--- | :--- | :--- |
| `BM_HeapConcurrentPushPop_p99` | 0.116 us | 0.683 us | 10 |

## Plug and Play (Build Instructions)

This project requires a C++20 compatible compiler and CMake (3.20+). Google Benchmark is automatically fetched and linked during configuration.

```bash
# Clone the repository
git clone [https://github.com/samraddhsingh20/ResourceAllocator.git](https://github.com/samraddhsingh20/ResourceAllocator.git)
cd ResourceAllocator

# Configure and compile with aggressive LTO optimizations
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j $(nproc)

# Run the latency benchmarks
./latency_bench
