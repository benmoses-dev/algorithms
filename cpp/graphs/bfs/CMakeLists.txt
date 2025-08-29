cmake_minimum_required(VERSION 3.10)
project(BFS LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3 -Wall -Wextra -Wshadow -Wconversion -Wfloat-equal -Wpedantic -Werror=return-type -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -DNDEBUG -pthread")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

add_executable(bfs tests/bfs.cpp)
add_executable(bfs_stress tests/bfs_stress.cpp)

target_include_directories(bfs PRIVATE include)
target_include_directories(bfs_stress PRIVATE include)
