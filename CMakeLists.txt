cmake_minimum_required(VERSION 3.4)
project(umthread_t)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp umthread.cpp)
add_executable(umthread_t ${SOURCE_FILES})