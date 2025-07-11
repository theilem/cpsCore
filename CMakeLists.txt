cmake_minimum_required (VERSION 3.13)
project (cpsCore)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
    message(FATAL_ERROR "Cannot compile, need c++17")
endif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -fPIC")

find_package (Eigen3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

set(CPP_REDIS_LIB cpp_redis)
set(TACOPIE_LIB tacopie)

set(SYSTEM_LIBS pthread
        dl)

set(SYSTEM_LIBS_STATIC pthread
        dl)

if (NOT APPLE)
    set(SYSTEM_LIBS ${SYSTEM_LIBS} rt)
    set(SYSTEM_LIBS_STATIC ${SYSTEM_LIBS_STATIC} rt.a)
endif(NOT APPLE)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
include_directories(${SUBMODULE_DIR}/cpp_redis/includes)
include_directories(${SUBMODULE_DIR}/cpp_redis/tacopie/includes)

add_subdirectory(src)

add_compile_definitions(_GLIBCXX_USE_NANOSLEEP)
add_definitions(-D_GLIBCXX_USE_NANOSLEEP)
add_subdirectory(${SUBMODULE_DIR}/cpp_redis)


set (TEST_SRCS ${TEST_SRCS})
add_subdirectory(tests)
if (CPS_CORE_DONT_BUILD_TESTS)
    set (TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
endif (CPS_CORE_DONT_BUILD_TESTS)


install(DIRECTORY include/cpsCore DESTINATION include)