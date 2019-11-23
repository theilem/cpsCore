set(TEST_SRCS TestRunner.cpp
        TestInfo.cpp)

add_subdirectory(Aggregation)
add_subdirectory(Configuration)
add_subdirectory(Synchronization)

include_directories(${SUBMODULE_DIR}/Catch2/single_include
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests EXCLUDE_FROM_ALL ${TEST_SRCS})

target_link_libraries(tests cpsCore)