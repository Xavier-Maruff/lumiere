cmake_minimum_required(VERSION 3.10)
project(lumiere VERSION 1.0 DESCRIPTION "Lumiere programming language")

set(CMAKE_CXX_STANDARD 20)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building with DEBUG_LOG expansion")
    add_compile_definitions(DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO: add llvm, flex, bison

include_directories("../common")

add_library(lu_parser SHARED
    driver.cpp
    ast.cpp
)