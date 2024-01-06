#include "utility.h"
#include <array>
#include <iostream>
#include <vector>

auto main()->int{
    std::cout << "hii pool \n";
    constexpr int buffer_size = 32;
    std::array<std::byte, buffer_size> arr{};
    util::printArrayContents("Initial state",arr);

    return 0;
}