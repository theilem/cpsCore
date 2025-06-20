set(CORE_SRCS)

add_subdirectory(Aggregation)
add_subdirectory(Configuration)
add_subdirectory(Synchronization)
add_subdirectory(Framework)
add_subdirectory(Logging)
add_subdirectory(Utilities)

add_library(cps_core ${CORE_SRCS})
#add_library(cps_core_static STATIC ${CORE_SRCS})

include_directories(${SUBMODULE_DIR}/Catch2/single_include
        ${CMAKE_CURRENT_SOURCE_DIR})

if (NO_DEBUG)
    target_compile_definitions(cps_core PRIVATE NODEBUG)
    message("Compiling with NODEBUG")
else ()

endif ()


target_link_libraries(cps_core
        ${SYSTEM_LIBS}
        ${CPP_REDIS_LIB}
        ${TACOPIE_LIB})


#target_link_libraries(cps_core_static
#        ${SYSTEM_LIBS_STATIC}
#        ${CPP_REDIS_LIB}
#        ${TACOPIE_LIB})

install(TARGETS cps_core
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)