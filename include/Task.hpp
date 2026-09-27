#pragma once
#include <cstdint>
#include <compare>

// Models the "real-time server task and workload" from the CV.
// Aligned to 64 bytes to prevent false sharing in cache lines during multi-threaded access.
struct alignas(64) Task {
    uint32_t priority;
    uint32_t payload_id;
    uint64_t timestamp;

    // C++20 Spaceship operator: Automatically generates <, >, <=, >=, ==, and !=.
    // It creates a strict weak ordering (compares priority, then payload_id, then timestamp).
    // This perfectly satisfies the std::totally_ordered concept required by the Min-Heap.
    auto operator<=>(const Task&) const = default;
};