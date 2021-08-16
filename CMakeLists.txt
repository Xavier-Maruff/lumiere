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

find_package( Boost COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_libraries(${Boost_LIBRARIES})

set(LLVM_PATH "/usr/lib/llvm-10")
link_directories(${LLVM_PATH}/lib)
include_directories(${LLVM_PATH}/include)

add_definitions(
    -D_GNU_SOURCE
    -D__STDC_CONSTANT_MACROS
    -D__STDC_FORMAT_MACROS
    -D__STDC_LIMIT_MACROS
)

add_executable(lumiere
    src/main.cpp
    src/log.cpp
    src/cli.cpp
    src/get_string.cpp
)

target_link_libraries(lumiere
    lu_parser
    LLVM-10
    LLVMCore
    LLVMCodeGen
    LLVMSupport
)

set_property(TARGET lumiere PROPERTY CXX_STANDARD 14)