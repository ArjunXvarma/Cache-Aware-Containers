#pragma once
#include <cstdlib>
#include <stdexcept>

namespace cac {

// Guarantees cache line alignment (64B)
class aligned_allocator {
public:
    static void* allocate(size_t bytes, size_t alignment) {
        void* ptr = nullptr;

        // posix_memalign guarantees cache line alignment
        if (posix_memalign(&ptr, alignment, bytes) != 0) throw std::bad_alloc();

        return ptr;
    }

    static void deallocate(void* ptr) {
        free(ptr);
    }
};

} // namespace cac
