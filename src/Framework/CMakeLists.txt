set(CORE_SRCS 	${CORE_SRCS}
				${CMAKE_CURRENT_SOURCE_DIR}/Helper.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/PluginManager.cpp
				PARENT_SCOPE)

add_library(uavap_framework_api	SHARED api/FrameworkAPI.cpp)
target_link_libraries(uavap_framework_api uavap_core)
					
									
install(TARGETS uavap_framework_api
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)