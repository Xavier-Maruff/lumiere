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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

find_package(LLVM REQUIRED)
link_libraries(
    LLVM
    LLVMCodeGen
    LLVMJIT
    LLVMExecutionEngine
    LLVMInterpreter
)

FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

BISON_TARGET(
    bison_parser bison.y ${CMAKE_CURRENT_BINARY_DIR}/bison.cpp
    COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit
)
FLEX_TARGET(
    flex_lexer flex.l ${CMAKE_CURRENT_BINARY_DIR}/flex.cpp
)
ADD_FLEX_BISON_DEPENDENCY(flex_lexer bison_parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(lu_parser STATIC
    driver.cpp
    ast.cpp
    ${BISON_bison_parser_OUTPUTS}
    ${FLEX_flex_lexer_OUTPUTS}
)