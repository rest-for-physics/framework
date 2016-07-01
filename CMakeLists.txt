cmake_minimum_required(VERSION 3.3)
project(REST VERSION 2.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(FindROOT)
include_directories(${ROOT_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native")

add_subdirectory(garfield)
include_directories(garfield/Include)

add_subdirectory(source)
