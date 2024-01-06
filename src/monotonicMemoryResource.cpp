// MonotonicMemoryResource.cpp

#include "monotonicMemoryResource.h"
#include <cstdint>
#include <iostream>

MonotonicMemoryResource::MonotonicMemoryResource(void* buffer, std::size_t buffer_size, std::pmr::memory_resource* upstream)
    : upstream_(upstream), buffer_(static_cast<char*>(buffer)), buffer_size_(buffer_size), offset_(0) {}


auto MonotonicMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) -> void* {
    // Calculate the padding needed to satisfy the alignment requirement
    size_t space = buffer_size_ - offset_;
    void* ptr = buffer_ + offset_;
    void* aligned_ptr = std::align(alignment, bytes, ptr, space);

    if (aligned_ptr == nullptr) {
        // Not enough space in the buffer, fallback to upstream allocator
        return upstream_->allocate(bytes, alignment);
    }

    // Update the offset to the new position after aligned allocation
    size_t aligned_space = static_cast<char*>(aligned_ptr) - static_cast<char*>(ptr);
    offset_ += aligned_space + bytes;
    return aligned_ptr;
}

void MonotonicMemoryResource::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) {
    // No deallocation for monotonic memory resource
    // Suppressing compiler warnings 
    (void)ptr;
    (void)bytes;
    (void)alignment;
}

auto MonotonicMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool  {
    return this == &other;
}