#ifndef MONOTONIC_MEMORY_RESOURCE_H
#define MONOTONIC_MEMORY_RESOURCE_H

#include <cstddef>
#include <memory_resource>

class MonotonicMemoryResource : public std::pmr::memory_resource {
public:
    MonotonicMemoryResource(void* buffer, std::size_t buffer_size, std::pmr::memory_resource* upstream);

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    std::pmr::memory_resource* upstream_;
    char* buffer_;
    std::size_t buffer_size_;
    std::size_t offset_;
};

#endif // MONOTONIC_MEMORY_RESOURCE_H

