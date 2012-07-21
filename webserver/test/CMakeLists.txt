file(GLOB sources
	"*.c"
	"*.h")

add_executable(test ${sources})

if(WEBSERVER_WITH_VLD)
	add_definitions(-DWEBSERVER_WITH_VLD)
	target_link_libraries(test vld.lib)
endif(WEBSERVER_WITH_VLD)
