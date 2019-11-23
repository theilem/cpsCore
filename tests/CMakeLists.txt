set(TEST_SRCS)

add_subdirectory(Aggregation)

include_directories(${SUBMODULE_DIR}/Catch2/include)

add_executable(tests EXCLUDE_FROM_ALL ${TEST_SRCS})