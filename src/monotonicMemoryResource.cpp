// MonotonicMemoryResource.cpp

#include "monotonicMemoryResource.h"
#include <cstdint>

MonotonicMemoryResource::MonotonicMemoryResource(void* buffer, std::size_t buffer_size, std::pmr::memory_resource* upstream)
    : upstream_(upstream), buffer_(static_cast<char*>(buffer)), buffer_size_(buffer_size), offset_(0) {}


void* MonotonicMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
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

void MonotonicMemoryResource::do_deallocate(void*, std::size_t, std::size_t ) {
    // No deallocation for monotonic memory resource
}

bool MonotonicMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}