#pragma once
#include <vector>
#include <mutex>
#include <utility>
#include <concepts>
#include "Task.hpp"

// "Engineered a concurrent Min-Heap architecture in C++"
template<std::totally_ordered T>
class ConcurrentMinHeap {
private:
    // "Implemented robust thread-safe synchronization via mutexes"
    // Padded to 64 bytes to isolate lock state from heap data in L1 cache.
    alignas(64) mutable std::mutex mtx_;
    alignas(64) std::vector<T> heap_;

    // "Reduced allocation time complexity to O(log n)"
    void heapify_up(size_t index) noexcept {
        while (index > 0) {
            size_t parent = (index - 1) >> 1; 
            if (heap_[index] < heap_[parent]) {
                std::swap(heap_[index], heap_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(size_t index) noexcept {
        const size_t size = heap_.size();
        while (true) {
            size_t left = (index << 1) + 1;
            size_t right = left + 1;
            size_t smallest = index;

            if (left < size && heap_[left] < heap_[smallest]) {
                smallest = left;
            }
            if (right < size && heap_[right] < heap_[smallest]) {
                smallest = right;
            }

            if (smallest != index) {
                std::swap(heap_[index], heap_[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    explicit ConcurrentMinHeap(size_t reserve_capacity = 200000) {
        // Pre-allocate to eliminate latency spikes caused by vector resizing inside the lock
        heap_.reserve(reserve_capacity);
    }

    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx_);
        heap_.push_back(std::move(value));
        heapify_up(heap_.size() - 1);
    }

    [[nodiscard]] bool try_pop(T& result) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        if (heap_.empty()) [[unlikely]] {
            return false;
        }
        
        result = std::move(heap_.front());
        
        if (heap_.size() > 1) [[likely]] {
            heap_.front() = std::move(heap_.back());
            heap_.pop_back();
            heapify_down(0);
        } else {
            heap_.pop_back();
        }
        
        return true;
    }
    
    [[nodiscard]] size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return heap_.size();
    }
};