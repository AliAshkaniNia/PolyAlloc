#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
#include <string_view>
#include <array>
#include <format>
#include <cstdint>

namespace util
{

    // Aligns the size to the nearest multiple of the alignment
    constexpr auto align(std::size_t size, std::size_t alignment) -> std::size_t
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    template <size_t X>
    void printArrayContents(std::string_view title,  std::array<std::byte, X>& data) {
        constexpr size_t numbers_per_line = 16;


        std::cout << std::format("==============={:^10}==============\n", title);
        std::cout << std::format("Buffer Address Start: 0x{:x}\n", reinterpret_cast<uintptr_t>(data.data()));


        for (size_t i = 0; i < X; ++i) {
            // Start a new line every 'numbers_per_line' elements
            if (i % numbers_per_line == 0) {
                if (i != 0) {
                    std::cout << '\n';
                }
                std::cout << std::format("(dec) {:02x}: ", i);
            }

            // Print the current element in decimal
            std::cout << std::format(" {:4}", static_cast<unsigned>(data[i]));
            
            // At the end of a line or the end of data, print hex and ascii values
            if ((i + 1) % numbers_per_line == 0 || i + 1 == X) {
                // Print hex values
                std::cout << '\n' << std::format("(hex) {:02x}: ", (i / numbers_per_line) * numbers_per_line);
                for (size_t j = i - (i % numbers_per_line); j <= i; ++j) {
                    std::cout << std::format("   {:-02x}", static_cast<unsigned>(data[j]));
                }
                
                // Print ASCII characters or octal values for non-printable characters
                std::cout << '\n' << std::format("(asc) {:02x}: ", (i / numbers_per_line) * numbers_per_line);
                for (size_t j = i - (i % numbers_per_line); j <= i; ++j) {
                    char ch = static_cast<char>(data[j]);
                    if (isprint(static_cast<unsigned char>(ch))) {
                        std::cout << std::format("   {} ", static_cast<char>(ch));
                    } else {
                        std::cout << std::format(" \\{:03o}", ch);
                        
                    }
                }
                std::cout << '\n';
            }
        }
    }
    

} // namespace util

#endif