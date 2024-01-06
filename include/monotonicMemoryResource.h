#ifndef MONOTONIC_MEMORY_RESOURCE_H
#define MONOTONIC_MEMORY_RESOURCE_H

#include <cstddef>
#include <memory_resource>

class MonotonicMemoryResource : public std::pmr::memory_resource {
public:
    MonotonicMemoryResource(void* buffer, std::size_t buffer_size, std::pmr::memory_resource* upstream);

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    std::pmr::memory_resource* upstream_;
    char* buffer_;
    std::size_t buffer_size_;
    std::size_t offset_;
};

#endif // MONOTONIC_MEMORY_RESOURCE_H


#if 0 

#include "utility.h"
#include <cassert>
#include <cstddef>
#include <memory_resource>

class MonotonicMemoryResource : public std::pmr::memory_resource {
private:
    char* buffer_;
    size_t buffer_size_;
    size_t current_ptr_;
    std::pmr::memory_resource* upstream_;
    
    void* align_ptr(void* ptr, std::size_t alignment) {
        auto aligned_size = util::align(reinterpret_cast<std::size_t>(ptr), alignment);
        return reinterpret_cast<void*>(aligned_size);
    }
protected:
    
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        // Calculate the padding needed to satisfy the alignment requirement
        size_t padding = (alignment - (reinterpret_cast<uintptr_t>(buffer_ + offset_) % alignment)) % alignment;
        size_t required_space = bytes + padding;

        // Check if the buffer has enough space, if not use the upstream resource
        if (offset_ + required_space > buffer_size_) {
            // Fallback to upstream allocator
            return upstream_->allocate(bytes, alignment);
        }

        char* ptr = buffer_ + offset_ + padding;
        offset_ += required_space;
        return ptr;
    }

    void do_deallocate(void* /* p */, std::size_t /* bytes */, std::size_t /* alignment */) override {
        // Deallocate is a no-op for the pre-allocated buffer
        // If the pointer didn't come from the pre-allocated buffer, deallocate using the upstream resource
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    MonotonicMemoryResource(void* buffer, size_t buffer_size, std::pmr::memory_resource* upstream)
        : buffer_(static_cast<char*>(buffer)), buffer_size_(buffer_size), current_ptr_(0), upstream_(upstream) {
        assert(buffer_ != nullptr && upstream_ != nullptr);
    }

    MonotonicMemoryResource(const MonotonicMemoryResource&) = delete;
    MonotonicMemoryResource& operator=(const MonotonicMemoryResource&) = delete;

    ~MonotonicMemoryResource() override {
        // The destructor doesn't need to do anything because the memory is managed externally.
    }
};

#endif 