#ifndef POOL_H
#define POOL_H

#include <memory_resource>
#include <cstddef>
#include <vector>

namespace memory
{
    class Pool
    {
    public:
        Pool(std::size_t blockSize, std::size_t maxChunkSize, std::pmr::memory_resource *upstream);
        ~Pool();

        void *allocate(std::size_t size, std::size_t alignment);
        void deallocate(void *p, std::size_t alignment);

        // Preventing copy construction and assignment
        Pool(const Pool &) = delete;
        Pool &operator=(const Pool &) = delete;

        Pool(Pool &&other) noexcept;
        Pool &operator=(Pool &&other) noexcept;

        std::size_t getBlockSize() const { return blockSize_; }

    private:
        struct Block
        {
            Block *next;
        };

        std::size_t blockSize_;
        std::size_t currentChunkSize_;
        std::size_t maxChunkSize_;
        Block *freeListHead_;
        std::vector<void *> chunks_; // To hold pointers to allocated chunks for cleanup
        std::pmr::memory_resource *upstreamResource_;

        void addChunk();
        void release();
    };
}
#endif // POOL_H