cmake_minimum_required(VERSION 3.10)
project(DATA_STRUCTURES LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# So I just put this here because I pretty much always build using GCC
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING 
        "Choose the build type (Debug, Release, RelWithDebInfo)" FORCE)
endif()

option(FAST_BUILD "Enable aggressive native optimizations. Not portable." OFF)

# Warnings may get annoying but it will help in the long run...
set(CMAKE_CXX_FLAGS_DEBUG 
    "-Og -g3 -Wall -Wextra -Wshadow -Wconversion -Wfloat-equal -Wpedantic -Werror=return-type -pthread")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -march=x86-64-v2 -mtune=generic -DNDEBUG -pthread")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

# Optimised and portable (on reasonably new hardware)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=x86-64-v2 -mtune=generic -DNDEBUG -pthread")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

if(FAST_BUILD)
    message(WARNING "FAST_BUILD=ON: Binaries may not run on other CPUs!")
    message(STATUS "FAST_BUILD=ON: Using -O3 -march=native (not portable)")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG -pthread")
endif()

# Try to find system Google Test first
find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "Using system Google Test")
else()
    message(STATUS "System Google Test not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Enable testing
enable_testing()

include(GoogleTest)

# Original executables
add_executable(fenwick_test tests/fenwick_test.cpp)
target_include_directories(fenwick_test PRIVATE include)
# Google Test executable
target_link_libraries(fenwick_test PRIVATE GTest::gtest GTest::gtest_main)
# Register the test with CTest
gtest_discover_tests(fenwick_test)

