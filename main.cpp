#include "utility.h"
#include "monotonicMemoryResource.h"
#include <array>
#include <iostream>
#include <vector>

auto main()->int{
    constexpr int monotonic_buffer_size = 32;
    {
        std::array<std::byte, monotonic_buffer_size> buffer{};
        std::cout<<"Regular usage of vector \n";

        util::printArrayContents("Initial Buffer state", buffer);
        MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());
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
        MonotonicMemoryResource memory_resource(buffer.data(), monotonic_buffer_size, std::pmr::null_memory_resource());

        std::pmr::vector<uint32_t> my_vector(&memory_resource);
        constexpr int push_elements = 4;

        for(unsigned char  i=0;i<push_elements;i++){
            my_vector.push_back(util::create_unsigned_num(i+1));
        }

        util::printArrayContents("After push back",buffer);
    }

    return 0;
}