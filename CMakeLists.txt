cmake_minimum_required(VERSION 3.16)
project(end2end_sst_gen)

set(CMAKE_CXX_STANDARD 14)

find_package(gflags REQUIRED)
add_library(SST_GEN SSTGen.cpp MergeChecker.cpp sst_reader.cc)
target_include_directories (SST_GEN PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(end2end_sst_gen sst_gen_test.cpp)
add_executable(fine_grained_random_access fine_grained_read.cpp)
target_link_libraries (end2end_sst_gen LINK_PUBLIC SST_GEN gflags)
target_link_libraries (fine_grained_random_access LINK_PUBLIC SST_GEN gflags)