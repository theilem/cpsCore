cmake_minimum_required (VERSION 3.0.0)
project (cpsCore)

set (CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SUBMODULE_DIR ${CMAKE_SOURCE_DIR}/extern)
include_directories(${SUBMODULE_DIR}/cpp_redis/includes)
include_directories(${SUBMODULE_DIR}/cpp_redis/tacopie/includes)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(${SUBMODULE_DIR}/cpp_redis)

install(DIRECTORY include/cpsCore DESTINATION include)