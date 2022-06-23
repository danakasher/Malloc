cmake_minimum_required(VERSION 3.22)
project(malloc)

set(CMAKE_CXX_STANDARD 11)

add_executable(malloc main.cpp malloc_1.cpp malloc_1.h malloc_2.cpp malloc_2.h)
