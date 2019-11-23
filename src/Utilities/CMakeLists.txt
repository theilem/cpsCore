set(CORE_SRCS ${CORE_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/LinearAlgebra.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalHandler/SignalHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TimeProvider/SystemTimeProvider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/MicroSimulator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/MultiThreadingScheduler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/Event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/EventBody.cpp
        PARENT_SCOPE)

