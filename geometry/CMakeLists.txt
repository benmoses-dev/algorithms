cmake_minimum_required(VERSION 3.10)
project(GEOMETRY LANGUAGES CXX)

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

add_executable(intersections tests/intersections.cpp)
target_include_directories(intersections PRIVATE include)

add_executable(pointinpolygon tests/pointinpolygon.cpp)
target_include_directories(pointinpolygon PRIVATE include)

add_executable(polygonarea tests/polygonarea.cpp)
target_include_directories(polygonarea PRIVATE include)
