cmake_minimum_required(VERSION 3.20)
project(end2end_sst_gen)

set(CMAKE_CXX_STANDARD 14)

add_executable(end2end_sst_gen main.cpp SSTGen.cpp SSTGen.h)
