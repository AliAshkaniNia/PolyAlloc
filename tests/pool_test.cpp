#include <gtest/gtest.h>
#include "poolMemoryResource.h"
#include "monotonicMemoryResource.h"

// Test fixture for PoolMemoryResource
class PoolMemoryResourceTest : public ::testing::Test {
protected:
    std::pmr::pool_options options;
    std::pmr::memory_resource* upstreamResource;
    std::pmr::memory_resource* nullUpstream;
    memory::PoolMemoryResource* poolMemoryResource = nullptr;
    memory::PoolMemoryResource* poolNullMemoryResource = nullptr;

    static constexpr size_t buffer_size = 4096;
    std::array<std::byte, buffer_size> buffer{};
    memory::MonotonicMemoryResource* monotonic_memory_resource;

    void SetUp() override {
        options.largest_required_pool_block = 1024; // Set an appropriate block size
        options.max_blocks_per_chunk = 10; // Set an appropriate block per chunk
        upstreamResource = std::pmr::new_delete_resource();
        poolMemoryResource = new memory::PoolMemoryResource(options, upstreamResource);


        monotonic_memory_resource = new memory::MonotonicMemoryResource(buffer.data(), buffer_size, std::pmr::null_memory_resource());
        poolNullMemoryResource = new memory::PoolMemoryResource(options, monotonic_memory_resource);
    }

    void TearDown() override {
        delete poolMemoryResource;
        poolMemoryResource = nullptr;

        delete poolNullMemoryResource;
        poolNullMemoryResource = nullptr;
        
        delete monotonic_memory_resource;
        monotonic_memory_resource = nullptr;


    }
};

// Test that memory is allocated from the correct pool based on size
TEST_F(PoolMemoryResourceTest, AllocateFromCorrectPool) {
    size_t blockSize = 128;
    size_t alignment = 8;

    void* firstAlloc = poolMemoryResource->allocate(blockSize, alignment);
    ASSERT_NE(firstAlloc, nullptr);

    // The pointer should be aligned
    EXPECT_EQ(reinterpret_cast<uintptr_t>(firstAlloc) % alignment, 0);

    // Allocate a larger block that should come from the next pool
    size_t largerBlockSize = 256;
    void* secondAlloc = poolMemoryResource->allocate(largerBlockSize, alignment);
    ASSERT_NE(secondAlloc, nullptr);

    // The second pointer should be aligned as well
    EXPECT_EQ(reinterpret_cast<uintptr_t>(secondAlloc) % alignment, 0);

    // Ensure that the two allocations are not the same, indicating they came from different pools
    EXPECT_NE(firstAlloc, secondAlloc);
}

// Test that memory allocated from pools is correctly deallocated
TEST_F(PoolMemoryResourceTest, DeallocateFromCorrectPool) {
    size_t blockSize = 128;
    size_t alignment = 8;

    void* ptr = poolMemoryResource->allocate(blockSize, alignment);
    ASSERT_NE(ptr, nullptr);

    poolMemoryResource->deallocate(ptr, blockSize, alignment);

    // The deallocate function does not return a value, but we can check if a subsequent allocation
    // returns the same memory address, indicating that the block was indeed deallocated and reused.
    void* newPtr = poolMemoryResource->allocate(blockSize, alignment);
    EXPECT_EQ(ptr, newPtr);
}

// Test that allocation falls back to the upstream resource for large sizes
TEST_F(PoolMemoryResourceTest, FallbackToUpstreamResource) {
    size_t largeBlockSize = options.largest_required_pool_block * 2; // Larger than the largest pool block
    size_t alignment = 8;


    EXPECT_THROW(auto ptr = poolNullMemoryResource->allocate(largeBlockSize, alignment), std::bad_alloc);
}

// Test that zero-byte allocation still returns a non-null pointer
TEST_F(PoolMemoryResourceTest, ZeroByteAllocation) {
    size_t zeroBytes = 0;
    size_t alignment = 8;

    void* ptr = poolMemoryResource->allocate(zeroBytes, alignment);
    ASSERT_NE(ptr, nullptr);
}

// Test that memory resources are correctly identified as equal
TEST_F(PoolMemoryResourceTest, MemoryResourcesEqual) {
    // Make a second pool memory resource with the same parameters
    memory::PoolMemoryResource secondPoolResource(options, upstreamResource);

    // Check that the same pool memory resource considers itself equal
    EXPECT_TRUE(poolMemoryResource->is_equal(*poolMemoryResource));
    // Check that two different pool memory resources do not consider themselves equal
    EXPECT_FALSE(poolMemoryResource->is_equal(secondPoolResource));
}

