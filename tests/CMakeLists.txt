set(TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)

add_subdirectory(Aggregation)
add_subdirectory(Configuration)
add_subdirectory(Framework)
add_subdirectory(Logging)
add_subdirectory(Synchronization)
add_subdirectory(Utilities)

include_directories(${SUBMODULE_DIR}/Catch2/single_include
        ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT CPS_CORE_DONT_BUILD_TESTS)
    set (TEST_SRCS ${TEST_SRCS} Test.cpp)
    add_executable(tests EXCLUDE_FROM_ALL ${TEST_SRCS})

    target_link_libraries(tests cps_core cps_framework_api)
else (NOT CPS_CORE_DONT_BUILD_TESTS)
    message(STATUS "Not building tests in cpsCore")
    set (TEST_SRCS ${TEST_SRCS} PARENT_SCOPE)
endif (NOT CPS_CORE_DONT_BUILD_TESTS)
