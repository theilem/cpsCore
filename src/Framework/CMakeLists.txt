set(CORE_SRCS 	${CORE_SRCS}
				${CMAKE_CURRENT_SOURCE_DIR}/PluginHelper.cpp
				PARENT_SCOPE)

add_library(cps_framework_api	 api/FrameworkAPI.cpp)
target_link_libraries(cps_framework_api cps_core)
					
									
install(TARGETS cps_framework_api
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)