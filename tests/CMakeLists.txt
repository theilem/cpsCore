set(TEST_SRCS Test.cpp)

add_subdirectory(Aggregation)
add_subdirectory(Configuration)
add_subdirectory(Framework)
add_subdirectory(Logging)
add_subdirectory(Synchronization)
add_subdirectory(Utilities)

include_directories(${SUBMODULE_DIR}/Catch2/single_include
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests EXCLUDE_FROM_ALL ${TEST_SRCS})

target_link_libraries(tests cps_core cps_framework_api)
