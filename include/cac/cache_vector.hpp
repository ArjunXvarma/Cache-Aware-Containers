#pragma once

#include <cstddef>
#include <new>
#include <utility>

#include "allocator.hpp"

namespace cac {

enum class GrowthPolicy {
    Double,
    OnePointFive
};

inline size_t next_capacity(size_t current, GrowthPolicy policy) {
    if (current == 0) return 1;

    size_t next;

    switch (policy) {
        case GrowthPolicy::Double:
            next = current * 2;
            break;

        case GrowthPolicy::OnePointFive:
            next = current + current / 2;
            break;
    }

    if (next <= current) {
        next = current + 1;
    }

    return next;
}

template<typename T, size_t Alignment = 64, GrowthPolicy GP = GrowthPolicy::Double>
class cache_vector {
public:
    cache_vector() = default;

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;

        T* new_data = static_cast<T*>(aligned_allocator::allocate(new_cap * sizeof(T), Alignment));

        size_t i = 0;
        try {
            for (; i < size_; i++) {
                new (&new_data[i]) T(std::move(data_[i]));
            }
        }
        catch(...) {
            // Rollback
            for (size_t j = 0; j < i; j++) 
                new_data[i].~T();
            
            aligned_allocator::deallocate(new_data);
            throw;
        }

        destroy_elements();
        aligned_allocator::deallocate(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) 
            reserve(next_capacity(capacity_, GP));

        new (&data_[size_]) T(value);
        size_++;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) 
            reserve(next_capacity(capacity_, GP));
        
        new (&data_[size_]) T(std::move(value));
        ++size_;
    }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }

    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }
    
    ~cache_vector() {
        destroy_elements();
        aligned_allocator::deallocate(data_);
    }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void destroy_elements() {
        for (size_t i = 0; i < size_; i++) {
            data_[i].~T();
        }
    }
};

}