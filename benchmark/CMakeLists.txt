cmake_minimum_required(VERSION 3.10)

project(benchmark_example)

# Add Google Benchmark library
find_package(benchmark REQUIRED)

# Add emb_rb source files
set(EMB_RB_SOURCES "../src/emb_rb.h"
                    "../src/emb_rb.c")

# Add benchmark executable
add_executable(benchmark_executable benchmark.cpp ${EMB_RB_SOURCES})

# Link Google Benchmark library
target_link_libraries(benchmark_executable benchmark::benchmark)

# Add include directories
target_include_directories(benchmark_executable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
