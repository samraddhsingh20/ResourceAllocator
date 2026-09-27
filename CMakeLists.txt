cmake_minimum_required(VERSION 3.20)
project(HighPerfResourceAllocator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# Aggressive optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG -pthread -Wall -Wextra")

# FIX: Disable Google Benchmark tests to prevent the CMake < 3.5 error from GoogleTest
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable testing" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable installation" FORCE)

include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)

add_library(core_heap INTERFACE)
target_include_directories(core_heap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(latency_bench benchmarks/latency_bench.cpp)
target_link_libraries(latency_bench PRIVATE core_heap benchmark::benchmark_main)