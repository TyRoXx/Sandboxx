file(GLOB sources
	"*.c"
	"*.h")

add_executable(webserver ${sources})

target_link_libraries(webserver file_system lua_script common ws_lua)

if(WIN32)
	target_link_libraries(webserver ws2_32)
else(WIN32)
	target_link_libraries(webserver pthread m)
endif(WIN32)

if(WEBSERVER_WITH_VLD)
	add_definitions(-DWEBSERVER_WITH_VLD)
	target_link_libraries(webserver vld.lib)
endif(WEBSERVER_WITH_VLD)
