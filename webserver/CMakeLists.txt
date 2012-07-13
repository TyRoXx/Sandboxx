cmake_minimum_required(VERSION 2.8)

project(webserver)

file(GLOB sources
	"*.c"
	"*.h")

add_executable(webserver ${sources})

if(WIN32)
	target_link_libraries(webserver ws2_32)
endif(WIN32)