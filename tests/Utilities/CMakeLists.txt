add_subdirectory(DataPresentation)
add_subdirectory(Scheduler)

set(TEST_SRCS 	${TEST_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/AngleTest.cpp
                PARENT_SCOPE)

configure_file(${PROJECT_SOURCE_DIR}/tests/Utilities/config/pm_test.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/pm_test.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/tests/Utilities/config/agg1.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/agg1.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/tests/Utilities/config/agg2.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/agg2.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/tests/Utilities/config/datahandling1.json ${CMAKE_BINARY_DIR}/tests/Utilities/config/datahandling1.json COPYONLY)