cmake_minimum_required(VERSION 3.9)
project(Language)

set(CMAKE_CXX_STANDARD 17)

add_executable(Language Language.cpp Compiler.cpp)