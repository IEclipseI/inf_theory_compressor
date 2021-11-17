cmake_minimum_required(VERSION 3.20)
project(inf_theory)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
add_executable(coder coder.cpp lib.h)
add_executable(decoder decoder.cpp lib.h)
