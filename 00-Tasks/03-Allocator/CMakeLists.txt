cmake_minimum_required(VERSION 3.14)
project(03-Allocator)

set(CMAKE_CXX_STANDARD 14)

include_directories(googletest/googletest/include googletest/googlemock/include)

add_executable(03-Allocator public.cpp private.cpp public_advanced.cpp private_advanced.cpp allocator.cpp allocator.h exceptions.h check.cpp)