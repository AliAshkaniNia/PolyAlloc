# download and populate Google Test from github
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)

# for Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(tests_monotonic monotonic_test.cpp ../src/monotonicMemoryResource.cpp )
add_executable(tests_utility utility_test.cpp   )

list( APPEND TEST_TARGETS tests_monotonic tests_utility )

include(GoogleTest)

foreach(target ${TEST_TARGETS})
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    target_include_directories(${target} PRIVATE ../include)

    target_link_libraries(${target} gtest_main gmock)
    gtest_discover_tests(${target})

endforeach()