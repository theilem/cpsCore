add_subdirectory(DataPresentation)
add_subdirectory(IPC)
add_subdirectory(Scheduler)

set(TEST_SRCS 	${TEST_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/AngleTest.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/FilterTest.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/TypeTraitsTest.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/EnumMap.cpp
                PARENT_SCOPE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/pm_test.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/pm_test.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/agg1.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/agg1.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/agg2.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/agg2.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/datahandling1.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/datahandling1.json COPYONLY)