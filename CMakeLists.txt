cmake_minimum_required (VERSION 3.0.0)
project (cpsCore)

set (CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SUBMODULE_DIR ${CMAKE_SOURCE_DIR}/extern)

add_subdirectory(src)
add_subdirectory(tests)

install(DIRECTORY include/cpsCore DESTINATION include)