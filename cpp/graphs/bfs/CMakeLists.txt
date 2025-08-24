cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(BFS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bfs tests/bfs.cpp)
add_executable(bfs_stress tests/bfs_stress.cpp)

target_include_directories(bfs PRIVATE include)
target_include_directories(bfs_stress PRIVATE include)
