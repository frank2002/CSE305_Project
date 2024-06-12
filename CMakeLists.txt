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
set(CMAKE_CXX_FLAGS_DEBUG "-pg")


# Include directories
include_directories(scripts)
include_directories(src)
include_directories(third_party/cpplog/include)

include_directories(/usr/include)

# Find all source files in src directory

file(GLOB_RECURSE SCRIPTS_FILES scripts/*.cpp)
file(GLOB_RECURSE SRC_FILES src/*.cpp)


# Main executable
add_executable(ParallelCrawler main.cpp  ${SCRIPTS_FILES} ${SRC_FILES})

add_subdirectory(third_party/curl-8.7.1)

target_link_libraries(ParallelCrawler PRIVATE libcurl)

add_subdirectory(third_party/cpplog/lib/cpplog)

target_link_libraries(ParallelCrawler PRIVATE cpplog)


# Link pthread for std::thread and other threading functionalities
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ParallelCrawler PRIVATE Threads::Threads)

target_link_libraries(ParallelCrawler PRIVATE /usr/lib/x86_64-linux-gnu/libre2.so)


# Specify the path to the cpplog library
# set(CPPLOG_LIB_PATH ${CMAKE_SOURCE_DIR}/third_party/cpplog/lib/libcpplog.a)

# # Verify the cpplog library exists
# if(NOT EXISTS ${CPPLOG_LIB_PATH})
#     message(FATAL_ERROR "cpplog library not found at ${CPPLOG_LIB_PATH}")
# endif()

# Link the cpplog static library
target_link_libraries(ParallelCrawler PRIVATE ${CPPLOG_LIB_PATH})

 
