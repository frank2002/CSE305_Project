cmake_minimum_required(VERSION 3.10)

# Project name
project(ParallelCrawler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Optimization flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Include directories
include_directories(src)
include_directories(scripts)


# Find all source files in src directory
file(GLOB_RECURSE SRC_FILES src/*.cpp)

file(GLOB_RECURSE SCRIPTS_FILES scripts/*.cpp)

# Main executable
add_executable(ParallelCrawler main.cpp ${SRC_FILES} ${SCRIPTS_FILES})

add_subdirectory(third_party/curl-8.7.1)

target_link_libraries(ParallelCrawler PRIVATE libcurl)


# Link pthread for std::thread and other threading functionalities
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ParallelCrawler PRIVATE Threads::Threads)

