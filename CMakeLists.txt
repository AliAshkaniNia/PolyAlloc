cmake_minimum_required(VERSION 3.20)
project(poolAllocationDemo)

set(EXEC_TARGET poolSample)

add_executable(${EXEC_TARGET})

target_sources(${EXEC_TARGET}
    PRIVATE
    main.cpp
    src/monotonicMemoryResource.cpp
)


target_compile_features(${EXEC_TARGET} PRIVATE cxx_std_20)
set_target_properties(${EXEC_TARGET} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(${EXEC_TARGET}  PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/include/
)

target_compile_options(${EXEC_TARGET} PRIVATE
        # set warnings for all build types
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror -fno-omit-frame-pointer>
         # O3 optimisation Release 
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3>
        # debug optimisation in Debug
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:-Og -g>
)

option(USE_ASAN "Use address sanitizers if available" ON)
option(USE_UBSAN "Use undefined behavior sanitizer if available" ON)
option(USE_CLANG_TIDY "Use clang-tidy if available" ON)
option(USE_CPPCHECK "Use cppcheck if available" ON)
option(BUILD_TESTS "Build Tests" ON)

if(USE_CLANG_TIDY)
        find_program(CLANGTIDY NAMES clang-tidy clang-tidy-17 clang-tidy-16 clang-tidy-15 clang-tidy-14 clang-tidy-13 clang-tidy-12 clang-tidy-11 clang-tidy-10)

        if(CLANGTIDY AND NOT (CLANGTIDY MATCHES "NOTFOUND"))
        message(STATUS "which clang-tidy: ${CLANGTIDY}")

        set_target_properties(${EXEC_TARGET} PROPERTIES
                CXX_CLANG_TIDY "${CLANGTIDY};-checks=modernize-*,readability-*,performance-*,bugprone-*,cppcoreguidelines-*,google-*,llvm-*,clang-analyzer-*"
                )
        endif()
endif(USE_CLANG_TIDY)

if(USE_CPPCHECK)
        find_program(CPPCHECK NAMES cppcheck HINTS $ENV{PROGRAMFILES}/cppcheck)
        if(CPPCHECK AND NOT (CPPCHECK MATCHES "NOTFOUND"))
        message(STATUS "which cppcheck: ${CPPCHECK}")

                set(CPPCHECK_CMD "${CPPCHECK}"
                "--enable=warning,performance,portability,style,information,missingInclude"
                "--template=gcc"
                ${CPPCHECK_SUPPRESS_LIST}
                )

        set_target_properties(${EXEC_TARGET} PROPERTIES
                CXX_CPPCHECK "${CPPCHECK_CMD}"
                )
        endif()
endif(USE_CPPCHECK)

set(HAS_ASAN FALSE)
set(HAS_UBSAN FALSE)

if (USE_ASAN)
    message("USE_ASAN set to ON. If you get errors about libasan, set USE_ASAN to OFF in the main CMakeLists.txt")
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libasan.so
            OUTPUT_VARIABLE LIBASAN_PATH
            RESULT_VARIABLE ASAN_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if ( USE_ASAN AND (${ASAN_RESULT} EQUAL 0) AND (NOT ${LIBASAN_PATH} STREQUAL ""))
        message("libasan found @${LIBASAN_PATH}")
        set(HAS_ASAN TRUE)
    endif ()

endif (USE_ASAN)

if (USE_UBSAN)
    message("USE_UBSAN set to ON. If you get errors about libubsan, set HAS_UBSAN to OFF in the main CMakeLists.txt")
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libubsan.so
            OUTPUT_VARIABLE LIBUBSAN_PATH
            RESULT_VARIABLE UBSAN_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if ( USE_UBSAN AND (${UBSAN_RESULT} EQUAL 0) AND (NOT ${LIBUBSAN_PATH} STREQUAL ""))
        message("libasan found @${LIBUBSAN_PATH}")
        set(HAS_UBSAN TRUE)
    endif ()

endif (USE_UBSAN)

if (HAS_ASAN AND USE_ASAN)
        target_compile_options(${EXEC_TARGET} PRIVATE -fsanitize=address -fsanitize-recover=address)
        target_link_options(${EXEC_TARGET} PRIVATE -fsanitize=address)
endif ()

if (USE_UBSAN AND USE_UBSAN)
        target_compile_options(${EXEC_TARGET} PRIVATE -fsanitize=undefined -fsanitize-recover=undefined)
        target_link_options(${EXEC_TARGET} PRIVATE -fsanitize=undefined)
endif ()

if(BUILD_TESTS)
add_subdirectory(tests)
endif(BUILD_TESTS)