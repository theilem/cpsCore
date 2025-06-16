set(CORE_SRCS ${CORE_SRCS}

        #General Utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/LinearAlgebra.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Packet.cpp

        #SignalHandler
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalHandler/SignalHandler.cpp

        #TimeProvider
        ${CMAKE_CURRENT_SOURCE_DIR}/TimeProvider/SystemTimeProvider.cpp

        #Scheduler
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/MicroSimulator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/ExternalSimulator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/MultiThreadingScheduler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/Event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/EventBody.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scheduler/ThreadPool/ThreadPoolScheduler.cpp

        #IPC
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/IPC.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/Subscription.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/detail/MessageQueueSubscriptionImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/detail/SharedMemorySubscriptionImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/detail/MessageQueuePublisherImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IPC/detail/SharedMemoryPublisherImpl.cpp

        #IDC
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/NetworkLayer/Serial/SerialHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/NetworkLayer/Serial/SerialNetworkLayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/NetworkLayer/Redis/RedisPublisher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/NetworkLayer/Redis/RedisSubscriber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/NetworkLayer/Redis/RedisNetworkLayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/IDC.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IDC/IDCSender.cpp

        #DataPresentation
        ${CMAKE_CURRENT_SOURCE_DIR}/DataPresentation/detail/Binary/BinaryFromArchive.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DataPresentation/detail/Binary/BinaryToArchive.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DataPresentation/detail/File/FileFromArchive.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DataPresentation/detail/File/FileToArchive.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DataPresentation/detail/BasicSerialization.cpp

        #Test
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/TestInfo.cpp

        PARENT_SCOPE)
