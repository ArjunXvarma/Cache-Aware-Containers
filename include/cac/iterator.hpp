#pragma once
#include <cstddef>
#include <iterator>
#include "proxy.hpp"

namespace cac {

class soa_particle_container;

class SoAIterator {
private:
    soa_particle_container* container_;
    size_t index_;
    
public:
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    using reference = ParticleProxy;

    SoAIterator(soa_particle_container* container, size_t index) : container_(container), index_(index) {}

    reference operator*() const;

    SoAIterator& operator++() {
        ++index_;
        return *this;
    }

    SoAIterator operator++(int) {
        SoAIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    SoAIterator& operator+=(difference_type n) {
        index_ += static_cast<size_t>(n);
        return *this;
    }

    SoAIterator operator+(difference_type n) const {
        return SoAIterator(container_, index_ + static_cast<size_t>(n));
    }

    difference_type operator-(const SoAIterator& other) const {
        return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
    }

    bool operator==(const SoAIterator& other) const {
        return index_ == other.index_;
    }

    bool operator!=(const SoAIterator& other) const {
        return index_ != other.index_;
    }
};

} // namespace cac
