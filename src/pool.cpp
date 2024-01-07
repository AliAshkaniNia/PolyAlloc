#include "pool.h"
#include <memory_resource>
#include <cassert>

Pool::Pool(std::size_t blockSize, std::size_t maxChunkSize, std::pmr::memory_resource* upstream)
    : blockSize_(blockSize),
      currentChunkSize_(blockSize),
      maxChunkSize_(maxChunkSize),
      freeListHead_(nullptr),
      upstreamResource_(upstream) {
    assert(blockSize > 0);
    assert(maxChunkSize >= blockSize);
    assert(upstreamResource_ != nullptr);
    addChunk(); // Initialize the pool with at least one chunk
}

// Move constructor
Pool::Pool(Pool&& other) noexcept
    : blockSize_(other.blockSize_),
      currentChunkSize_(other.currentChunkSize_),
      maxChunkSize_(other.maxChunkSize_),
      freeListHead_(other.freeListHead_),
      chunks_(std::move(other.chunks_)),
      upstreamResource_(other.upstreamResource_) {
    other.freeListHead_ = nullptr;
    other.currentChunkSize_ = 0;
    other.upstreamResource_ = nullptr;
}

// Move assignment operator
Pool& Pool::operator=(Pool&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        release();
        // Move resources from other to this
        blockSize_ = other.blockSize_;
        currentChunkSize_ = other.currentChunkSize_;
        maxChunkSize_ = other.maxChunkSize_;
        freeListHead_ = other.freeListHead_;
        chunks_ = std::move(other.chunks_);
        upstreamResource_ = other.upstreamResource_;
        // Invalidate the other object
        other.freeListHead_ = nullptr;
        other.currentChunkSize_ = 0;
        other.upstreamResource_ = nullptr;
    }
    return *this;
}

Pool::~Pool() {
    release();
}

void Pool::release(){
    for (auto chunk : chunks_) {
        upstreamResource_->deallocate(chunk, currentChunkSize_, alignof(std::max_align_t));
    }
}

void Pool::addChunk() {
    currentChunkSize_ = std::min(currentChunkSize_ * 2, maxChunkSize_);
    void* chunk = upstreamResource_->allocate(currentChunkSize_, alignof(std::max_align_t));
    chunks_.push_back(chunk);

    // Split chunk into blocks and link them
    uintptr_t chunkStart = reinterpret_cast<uintptr_t>(chunk);
    uintptr_t chunkEnd = chunkStart + currentChunkSize_;

    for (uintptr_t blockAddr = chunkStart; blockAddr < chunkEnd; blockAddr += blockSize_) {
        Block* block = reinterpret_cast<Block*>(blockAddr);
        block->next = freeListHead_;
        freeListHead_ = block;
    }
}

void* Pool::allocate(std::size_t size, std::size_t alignment) {
    (void)alignment;//suppressing warnings 

    assert(size <= blockSize_);
    if (freeListHead_ == nullptr) {
        addChunk();
    }

    Block* block = freeListHead_;
    freeListHead_ = block->next;
    return block;
}

void Pool::deallocate(void* p, std::size_t alignment) {
    (void)alignment;//suppressing warnings 
    Block* block = static_cast<Block*>(p);
    block->next = freeListHead_;
    freeListHead_ = block;
}