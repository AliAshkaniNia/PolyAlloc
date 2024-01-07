# PolyAlloc - Polymorphic Allocators Practice

## Introduction

This repository is dedicated to re-implementing standard polymorphic memory allocators as a means to deepen understanding of memory allocation mechanisms in C++. Polymorphic allocators allow for custom memory management strategies which can be optimized for specific use cases, leading to potentially improved performance and more efficient use of resources.

If you are not familiar with polymorphic memory allocators or the concepts behind them, you may find it helpful to visit this *fantastic* YouTube playlist: [PMR And Allocators](https://youtube.com/playlist?list=PLs3KjaCtOwSYX3X0L36NgwK0pxZZavDSF). It provides a comprehensive guide that should bring you up to speed with the necessary background to understand the implementations provided in this repository.


## Building the Project 
To build the project, clone the repository and execute the following commands in your terminal:
```
mkdir build 
cd build 
cmake ..
cmake --build . -j${nproc}
```
The following options can be enabled:

- `USE_ASAN` : Utilize address sanitizers if available
- `USE_UBSAN` : Employ undefined behavior sanitizer if available
- `USE_CLANG_TIDY` : Utilize clang-tidy if available
- `USE_CPPCHECK` : Employ cppcheck if available
- `BUILD_TESTS` : Build Tests

To use these options, execute:

```bash
cmake -DBUILD_TESTS=ON -USE_UBSAN=OFF ..
cmake --build . -j$(nproc)
```

## Executing Unit Tests

To run the unit tests, you first need to enable the `BUILD_TESTS` switch as mentioned above. After the project build is complete, the test executables will be found in the `build/tests/` directory. For instance, to run the pool test, use the following command:
```bash
cd build 
./tests/tests_pool
```


## Implementations

### MonotonicMemoryResource

This is a reimplementation of `std::pmr::monotonic_buffer_resource`. It allocates from a fixed buffer until it is exhausted, then falls back to an upstream allocator.

Key characteristics:

- Allocations are served from a fixed buffer until it is exhausted
- Once exhausted, upstream allocator is used
- No deallocation supported
- Satisfies alignment requirements by padding if needed
- Updates an offset to track progress through buffer

### PoolMemoryResource 

This is a reimplementation of `std::pmr::unsynchronized_pool_resource`. It manages a collection of pools, each serving blocks of a fixed size.

Key characteristics:

- Owns allocated memory, frees on destruction
- Collection of pools serving different block sizes
- Each pool manages chunks divided into uniform blocks

    ```
    Pool 1:
    Chunk 1:
    [Block][Block]...

    Chunk 2: 
    [Block][Block]...
    ```

- Allocate dispatches to pool with smallest sufficient block
- Pool exhaustion triggers new chunk from upstream
- Largest block size and max chunk size configurable
- Oversized allocations served directly by upstream

The use of `std::max_align_t` ensures that the pool's blocks are aligned for any type.

## Design

Both resources implement the polymorphic `std::pmr::memory_resource` interface. This allows them to be used interchangeably with other standard allocators like `std::pmr::new_delete_resource`.

`MonotonicMemoryResource` simply manages an offset into a provided buffer, aligning allocations as needed. 

`PoolMemoryResource` maintains a collection of `Pool` objects, each representing a pool for blocks of a certain size. On allocation, it finds the pool that serves the smallest block size that satisfies the request. The `Pool` handles chunk allocation from upstream and carving them into blocks.

## Example Usage

Here is an example demonstrating usage of the `MonotonicMemoryResource`:

```cpp
constexpr int monotonic_buffer_size = 32;
std::array<std::byte, monotonic_buffer_size> buffer{};

memory::MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());

constexpr int vec_size = 4;
std::pmr::vector<uint32_t> my_vector(vec_size, &memory_resource);

for(unsigned char i=0; i<vec_size; i++){
  my_vector[i] = i+1;
}

```

This initializes a `monotonic_buffer_resource` backed by the `buffer` array. A `pmr::vector` is then constructed using it which allocates space for 4 elements. `std::pmr::null_memory_resource()` is the upstream memory resource in this example.



## Requirements & Dependencies

A C++20 compiler is required to compile this project, as it utilizes C++20 features like  `std::format`. 

This program is self-contained and does not require any third-party libraries for compilation. [GoogleTest](https://github.com/google/googletest) is used for unit testing and is automatically downloaded by CMake.