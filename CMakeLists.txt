cmake_minimum_required(VERSION 3.0.0)
project(book-metadata VERSION 0.1.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
message("cmake toolchain file" ${CMAKE_TOOLCHAIN_FILE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty)
add_subdirectory(src)
