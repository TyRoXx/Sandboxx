file(GLOB sources
	"*.c"
	"*.h")

add_executable(webserver ${sources})

target_link_libraries(webserver file_system lua_script sub_directory http common ws_lua)

if(WIN32)
	target_link_libraries(webserver ws2_32)
else(WIN32)
	target_link_libraries(webserver pthread m)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(webserver dl)
	endif()
endif(WIN32)

if(WEBSERVER_WITH_VLD)
	add_definitions(-DWEBSERVER_WITH_VLD)
	target_link_libraries(webserver vld.lib)
endif(WEBSERVER_WITH_VLD)
