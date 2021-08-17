cmake_minimum_required(VERSION 3.10)
project(lumiere VERSION 1.0 DESCRIPTION "Lumiere programming language")

set(CMAKE_CXX_STANDARD 20)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building with DEBUG_LOG expansion")
    add_compile_definitions(DEBUG)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO: add llvm, flex, bison

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

set(LLVM_PATH "/usr/lib/llvm-10")
include_directories(${LLVM_PATH}/include)

add_definitions(
    -D_GNU_SOURCE
    -D__STDC_CONSTANT_MACROS
    -D__STDC_FORMAT_MACROS
    -D__STDC_LIMIT_MACROS
)

FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

BISON_TARGET(
    bison_parser bison.y ${CMAKE_CURRENT_BINARY_DIR}/bison.cpp
    COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit COMPILE_FLAGS -Wcounterexamples
    VERBOSE bison_log.txt
)
FLEX_TARGET(
    flex_lexer flex.l ${CMAKE_CURRENT_BINARY_DIR}/flex.cpp
)
ADD_FLEX_BISON_DEPENDENCY(flex_lexer bison_parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(lu_parser STATIC
    driver.cpp
    ast.cpp
    llvm_inst.cpp
    cmp_time_extensible.cpp
    ${BISON_bison_parser_OUTPUTS}
    ${FLEX_flex_lexer_OUTPUTS}
)

set_property(TARGET lu_parser PROPERTY CXX_STANDARD 14)