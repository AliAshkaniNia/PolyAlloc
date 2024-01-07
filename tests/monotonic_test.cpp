#include <gtest/gtest.h>
#include "monotonicMemoryResource.h"

// Test fixture for MonotonicMemoryResource
class MonotonicMemoryResourceTest : public ::testing::Test {
protected:
    static constexpr size_t buffer_size = 1024;

    std::array<std::byte, buffer_size> buffer{};

    memory::MonotonicMemoryResource* memory_resource;

    void SetUp() override {
        memory_resource = new memory::MonotonicMemoryResource(buffer.data(), buffer_size, std::pmr::null_memory_resource());
    }

    void TearDown() override {
        delete memory_resource;
    }
};

// Test multiple allocations and ensure they don't overlap
TEST_F(MonotonicMemoryResourceTest, MultipleAllocations) {
    void* ptr1 = memory_resource->allocate(128, 8);
    void* ptr2 = memory_resource->allocate(256, 8);

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(static_cast<char*>(ptr2) - static_cast<char*>(ptr1), 128);
}

// Test that allocated memory is aligned correctly
TEST_F(MonotonicMemoryResourceTest, Alignment) {
    void* ptr = memory_resource->allocate(64, alignof(double));
    uintptr_t ptr_addr = reinterpret_cast<uintptr_t>(ptr);
    EXPECT_EQ(ptr_addr % alignof(double), 0);
}

// Test that allocation does not exceed buffer size
TEST_F(MonotonicMemoryResourceTest, OutOfSpace) {
    void* ptr1 = memory_resource->allocate(buffer_size - 16);
    ASSERT_NE(ptr1, nullptr);

    // This should trigger the use of the upstream memory resource.
    EXPECT_THROW(auto it = memory_resource->allocate(64), std::bad_alloc);
}

// Test that deallocate does not actually free memory in a monotonic buffer
TEST_F(MonotonicMemoryResourceTest, DeallocateNoOp) {
    unsigned char* ptr1 = static_cast<unsigned char*>(memory_resource->allocate(64,1));
    unsigned char* ptr2 = ptr1;
    for(int i=0; i<64;i++){
        *(ptr1++) = i;
    }

    memory_resource->deallocate(ptr1, 64);
    for(int i=0; i<64;i++){
        EXPECT_EQ(*(ptr2++), i);
    }
}

// Test that requesting zero bytes returns a non-null pointer
TEST_F(MonotonicMemoryResourceTest, ZeroByteAllocation) {
    void* ptr = memory_resource->allocate(0);
    ASSERT_NE(ptr, nullptr);
}
