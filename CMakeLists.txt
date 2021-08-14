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

add_subdirectory(src/lu_parser)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lu_parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lu_vm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/common)
include_directories(${CMAKE_BINARY_DIR}/src/lu_parser)

#link_libraries(uv)

add_executable(lumiere
    src/main.cpp
)

target_link_libraries(lumiere lu_parser)