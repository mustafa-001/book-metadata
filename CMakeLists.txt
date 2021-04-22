cmake_minimum_required(VERSION 3.0.0)
project(book-metadata VERSION 0.1.0)

# Use this snippet *after* PROJECT(xxx):
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# SET(CMAKE_C_COMPILER "/usr/bin/gcc")
# SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
# SET(ENV{CROSS_TC} "arm-linux-gnueabihf")
# include(CMakeCross.cmake)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Woverloaded-virtual -Wswitch-enum -Wcast-qual -Wpedantic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g --std=gnu++2a  ")

include_directories(/usr/include)
include_directories(/mingw64/include/freetype)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include")



add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(thirdparty)
