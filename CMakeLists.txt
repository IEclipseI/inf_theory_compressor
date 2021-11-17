cmake_minimum_required(VERSION 3.20)
project(inf_theory)

set(CMAKE_CXX_STANDARD 17)

add_executable(coder coder.cpp lib.h)
add_executable(decoder decoder.cpp lib.h)
