#ifndef POOL_MEMORY_RESOURCE_H
#define POOL_MEMORY_RESOURCE_H

#include "pool.h"
#include <memory_resource>
#include <vector>

class PoolMemoryResource : public std::pmr::memory_resource {
public:
    explicit PoolMemoryResource(const std::pmr::pool_options& opts, std::pmr::memory_resource* upstream);
    // Pools are automatically cleaned up by their destructors
    ~PoolMemoryResource() override = default;

    // Memory resources should not be copyable
    PoolMemoryResource(const PoolMemoryResource&) = delete;
    PoolMemoryResource& operator=(const PoolMemoryResource&) = delete;
    
    PoolMemoryResource(PoolMemoryResource&&) noexcept;
    PoolMemoryResource& operator=(PoolMemoryResource&&) noexcept;
protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    ///TODO: Use shared_ptr and use default move constructors 
    std::pmr::memory_resource* upstreamResource_;
    std::vector<Pool> pools_;
    std::size_t largestBlockSize_;


};

#endif // POOL_MEMORY_RESOURCE_H