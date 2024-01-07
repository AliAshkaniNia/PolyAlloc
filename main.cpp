#include "utility.h"
#include "monotonicMemoryResource.h"
#include "poolMemoryResource.h"
#include <array>
#include <iostream>
#include <vector>

auto main()->int{
    constexpr int monotonic_buffer_size = 32;
    {
        std::array<std::byte, monotonic_buffer_size> buffer{};
        std::cout<<"Regular usage of vector \n";

        util::printArrayContents("Initial Buffer state", buffer);
        memory::MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());
        constexpr int vec_size = 4;
        std::pmr::vector<uint32_t> my_vector(vec_size, &memory_resource);
        for(unsigned char  i=0;i<vec_size;i++){
            my_vector[i] = util::create_unsigned_num(i+1); 
        }
        util::printArrayContents("After assignment",buffer);
    }

    {
        std::array<std::byte, monotonic_buffer_size> buffer{};
        std::cout<<"Showcasing vector's push_back memory footprint\n";
        util::printArrayContents("Initial Buffer state",buffer);
        memory::MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());

        std::pmr::vector<uint32_t> my_vector(&memory_resource);
        constexpr int push_elements = 4;

        for(unsigned char  i=0;i<push_elements;i++){
            my_vector.push_back(util::create_unsigned_num(i+1));
        }

        util::printArrayContents("After push back",buffer);
    }
    {
        std::pmr::pool_options opts;
        opts.max_blocks_per_chunk = 4; // Increase geometrically by this factor
        opts.largest_required_pool_block = 64; // Largest block size to handle in pool
        
        constexpr int monotonic_buffer_size = 512;

        std::array<std::byte, monotonic_buffer_size> buffer{};
        memory::MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());

        std::cout<<"Pool usage \n";

        // Create the pool resource with options and upstream resource
        memory::PoolMemoryResource pool(opts, &memory_resource);
        util::printArrayContents("Before assignment",buffer);

        constexpr int test_elements = 2;

        std::pmr::vector<int> my_vector(test_elements, &pool);
        for(int i=0;i<test_elements;i++){
            my_vector[i]=util::create_unsigned_num(i+1);
        }

        std::pmr::vector<unsigned char> my_char_vector(test_elements, &pool);
        for(int i=0;i<test_elements;i++){
            my_char_vector[i]=i+1;
        }
        util::printArrayContents("After assignment",buffer);
    }

    return 0;
}