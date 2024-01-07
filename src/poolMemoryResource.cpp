
#include "poolMemoryResource.h"
#include "utility.h"
#include <algorithm>
#include <cassert>
#include <utility>

PoolMemoryResource::PoolMemoryResource(const std::pmr::pool_options& opts, std::pmr::memory_resource* upstream)
    : upstreamResource_(upstream != nullptr ? upstream : std::pmr::get_default_resource()),
      largestBlockSize_(opts.largest_required_pool_block) {
    assert(upstreamResource_ != nullptr);

    std::size_t blockSize = 8;

    while (blockSize <= opts.largest_required_pool_block) {
        pools_.emplace_back(blockSize, opts.max_blocks_per_chunk * blockSize, upstream);
        blockSize *= 2;
    }
}

PoolMemoryResource::PoolMemoryResource(PoolMemoryResource&& other) noexcept{
    pools_ = std::move(other.pools_);
    upstreamResource_ = std::exchange(other.upstreamResource_, nullptr);
    largestBlockSize_ = std::exchange(other.largestBlockSize_, 0);
}

PoolMemoryResource& PoolMemoryResource::operator=(PoolMemoryResource&& other) noexcept{
    if (this != &other) {
        pools_ = std::move(other.pools_);
        upstreamResource_ = std::exchange(other.upstreamResource_, nullptr);
        largestBlockSize_ = std::exchange(other.largestBlockSize_, 0);
    }
    return *this;
}

auto PoolMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) -> void*{
    bytes = util::align(bytes, alignment);
    auto pool = std::lower_bound(pools_.begin(), pools_.end(), bytes, [](const Pool& p, std::size_t size) {
            return p.getBlockSize() < size;
        });

    if(pool == pools_.end()){
        return upstreamResource_->allocate(bytes, alignment);
    }

    return pool->allocate(bytes, alignment);
}

void PoolMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    bytes = util::align(bytes, alignment);
    auto pool = std::lower_bound(pools_.begin(), pools_.end(), bytes, [](const Pool& p, std::size_t size) {
            return p.getBlockSize() < size;
        });

    if(pool == pools_.end()){
        upstreamResource_->deallocate(p, bytes, alignment);
        return;
    }
    pool->deallocate(p, alignment);
}

auto PoolMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool {
    return this == &other;
}


