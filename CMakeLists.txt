cmake_minimum_required(VERSION 3.20)
project(poolAllocationDemo)

set(EXEC_TARGET poolSample)

add_executable(${EXEC_TARGET})

target_sources(${EXEC_TARGET}
    PRIVATE
    main.cpp
)


target_compile_features(${EXEC_TARGET} PRIVATE cxx_std_17)
set_target_properties(${EXEC_TARGET} PROPERTIES CXX_EXTENSIONS OFF)


target_compile_options(${EXEC_TARGET} PRIVATE
        # set warnings for all build types
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror -fno-omit-frame-pointer>
         # O3 optimisation Release 
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3>
        # debug optimisation in Debug
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:-Og -g>
)

option(BUILD_TESTS "Build Tests" ON)

if(BUILD_TESTS)
add_subdirectory(tests)
endif(BUILD_TESTS)