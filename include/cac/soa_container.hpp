#pragma once
#include <cstddef>
#include <iterator>
#include "allocator.hpp"
#include "particle.hpp"
#include "proxy.hpp"
#include "iterator.hpp"

namespace cac {

struct SoAStorage {
    float* x = nullptr;
    float* y = nullptr;
    float* z = nullptr;

    float* vx = nullptr;
    float* vy = nullptr;
    float* vz = nullptr;
};

// All arrays MUST - 
// Have same capacity
// Be aligned
// Be resized together

class soa_particle_container {
private:
    SoAStorage data_;
    size_t size_;
    size_t capacity_;

    static constexpr size_t Alignment = 64;

    void deallocate() {
        if (data_.x)  aligned_allocator::deallocate(data_.x);
        if (data_.y)  aligned_allocator::deallocate(data_.y);
        if (data_.z)  aligned_allocator::deallocate(data_.z);

        if (data_.vx) aligned_allocator::deallocate(data_.vx);
        if (data_.vy) aligned_allocator::deallocate(data_.vy);
        if (data_.vz) aligned_allocator::deallocate(data_.vz);

        data_ = {}; // reset to nullptr
    }

    void allocate(size_t cap) {
        data_.x = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));
        data_.y = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));
        data_.z = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));

        data_.vx = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));
        data_.vy = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));
        data_.vz = static_cast<float*>(aligned_allocator::allocate(cap * sizeof(float), Alignment));
    }

public:
    soa_particle_container() : size_(0), capacity_(0) {}
    
    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;

        // Make a new variable for storage
        SoAStorage new_data{};
        new_data.x = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));
        new_data.y = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));
        new_data.z = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));

        new_data.vx = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));
        new_data.vy = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));
        new_data.vz = static_cast<float*>(aligned_allocator::allocate(new_cap * sizeof(float), Alignment));

        // Copy data into new storage
        for (size_t i = 0; i < size_; ++i) {
            new_data.x[i]  = data_.x[i];
            new_data.y[i]  = data_.y[i];
            new_data.z[i]  = data_.z[i];

            new_data.vx[i] = data_.vx[i];
            new_data.vy[i] = data_.vy[i];
            new_data.vz[i] = data_.vz[i];
        }

        deallocate();
        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(float x, float y, float z, float vx, float vy, float vz) {
        if (size_ == capacity_) {
            size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
            reserve(new_cap);
        }

        data_.x[size_] = x;
        data_.y[size_] = y;
        data_.z[size_] = z;

        data_.vx[size_] = vx;
        data_.vy[size_] = vy;
        data_.vz[size_] = vz;

        size_++;
    }

    void push_back(const Particle& p) {
        push_back(p.x, p.y, p.z, p.vx, p.vy, p.vz);
    }

    ParticleProxy operator[](size_t i) {
        return at(i);
    }

    ConstParticleProxy operator[](size_t i) const {
        return {
            data_.x[i],
            data_.y[i],
            data_.z[i],
            data_.vx[i],
            data_.vy[i],
            data_.vz[i]
        };
    }

    ParticleProxy at(size_t i) {
        return {
            data_.x[i],
            data_.y[i],
            data_.z[i],

            data_.vx[i],
            data_.vy[i],
            data_.vz[i]
        };
    }

    SoAIterator begin() {
        return SoAIterator(this, 0);
    }

    SoAIterator end() {
        return SoAIterator(this, size_);
    }

    ~soa_particle_container() {
        deallocate();
    }
};

inline ParticleProxy SoAIterator::operator*() const {
    return container_->at(index_);
}

} // namespace cac
