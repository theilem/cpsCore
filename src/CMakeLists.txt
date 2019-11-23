set(CORE_SRCS)

add_subdirectory(Aggregation)
add_subdirectory(Configuration)
add_subdirectory(Synchronization)
add_subdirectory(Logging)
add_subdirectory(Utilities)



add_library(cpsCore SHARED ${CORE_SRCS})

target_link_libraries(cpsCore   pthread
                                rt
                                Eigen3::Eigen
                                Boost::boost)