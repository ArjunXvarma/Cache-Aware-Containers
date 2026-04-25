#pragma once

#include <cstddef>
#include <new>
#include <utility>

namespace cac {
    
template<typename T>
class aos_container {
public:
    aos_container() = default;

    ~aos_container() {
        destroy_elements();
        operator delete[](data_);
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;

        T* new_data = static_cast<T*>(operator new[](new_cap * sizeof(T)));

        for (size_t i = 0; i < size_; i++) {
            new (&new_data[i]) T(std::move(data_[i]));
            data_[i].~T();
        }

        operator delete[](data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) 
            reserve(capacity_ == 0 ? 1 : 2 * capacity_);

        new (&data_[size_]) T(value);
        size_++;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) 
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        
        new (&data_[size_]) T(std::move(value));
        ++size_;
    }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }

    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void destroy_elements() {
        for (size_t i = 0; i < size_; i++) 
            data_[i].~T();
    }
};

} // namespace cac
