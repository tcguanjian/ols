cmake_minimum_required(VERSION 3.10)
project(SimpleMatrix)

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(SimpleMatrix main.cpp SimpleMatrix.cpp SimpleMatrix.h)