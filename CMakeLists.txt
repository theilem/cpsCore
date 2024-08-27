cmake_minimum_required (VERSION 3.13)
project (cpsCore)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
    message(FATAL_ERROR "Cannot compile, need c++17")
endif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC -static-libstdc++")

find_package (Eigen3 REQUIRED NO_MODULE)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

if (DEPLOY_PATH)
    message("Deploying to ${DEPLOY_PATH}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -static-libstdc++")
    set(COMPILE_REDIS 1)
    link_directories(BEFORE ${DEPLOY_PATH}/lib/)
    set(CMAKE_INSTALL_PREFIX ${DEPLOY_PATH})
endif(DEPLOY_PATH)


if (NOT COMPILE_REDIS)
    find_library(CPP_REDIS_LIBRARY cpp_redis)
    find_library(TACOPIE_LIBRARY tacopie)
endif()

if (CPP_REDIS_LIBRARY)
    set(CPP_REDIS_LIB ${CPP_REDIS_LIBRARY})
else()
    set(CPP_REDIS_LIB cpp_redis)
endif(CPP_REDIS_LIBRARY)

if (TACOPIE_LIBRARY)
    set(TACOPIE_LIB ${TACOPIE_LIBRARY})
else()
    set(TACOPIE_LIB tacopie)
endif(TACOPIE_LIBRARY)

set(SYSTEM_LIBS pthread
        dl)

set(SYSTEM_LIBS_STATIC pthread
        dl)

if (NOT APPLE)
    set(SYSTEM_LIBS ${SYSTEM_LIBS} rt)
    set(SYSTEM_LIBS_STATIC ${SYSTEM_LIBS_STATIC} rt.a)
endif(NOT APPLE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)


include_directories(${CMAKE_SOURCE_DIR}/include)

set(SUBMODULE_DIR ${CMAKE_SOURCE_DIR}/extern)
include_directories(${SUBMODULE_DIR}/cpp_redis/includes)
include_directories(${SUBMODULE_DIR}/cpp_redis/tacopie/includes)

add_subdirectory(src)
add_subdirectory(tests)

if (NOT CPP_REDIS_LIBRARY)
    add_compile_definitions(_GLIBCXX_USE_NANOSLEEP)
    add_definitions(-D_GLIBCXX_USE_NANOSLEEP)
    add_subdirectory(${SUBMODULE_DIR}/cpp_redis)

    if (DEPLOY_PATH)
        add_subdirectory(${SUBMODULE_DIR}/cpp_redis/tacopie)
    endif(DEPLOY_PATH)
endif(NOT CPP_REDIS_LIBRARY)

install(DIRECTORY include/cpsCore DESTINATION include)