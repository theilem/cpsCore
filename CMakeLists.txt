cmake_minimum_required (VERSION 3.0.0)
project (cpsCore)

set (CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SUBMODULE_DIR ${CMAKE_SOURCE_DIR}/extern)

add_subdirectory(src)
add_subdirectory(tests)