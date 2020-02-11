cmake_minimum_required(VERSION 3.15)
project(sTaskFlow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0")

FIND_PACKAGE(Threads)

add_executable(demo1 demo1.cpp)
TARGET_LINK_LIBRARIES(demo1 ${CMAKE_THREAD_LIBS_INIT})
