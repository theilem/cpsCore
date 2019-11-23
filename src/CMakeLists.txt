set(CORE_SRCS)

add_subdirectory(Aggregation)
add_subdirectory(Logging)



add_library(cpsCore SHARED ${CORE_SRCS})