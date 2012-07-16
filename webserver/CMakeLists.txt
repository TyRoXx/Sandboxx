cmake_minimum_required(VERSION 2.8)

project(webserver)

if(MSVC)
	set(WEBSERVER_WITH_VLD false CACHE BOOL "Visual Leak Detector, finds memory leaks with Visual C++ (vld.codeplex.com)")
endif(MSVC)

if(UNIX)
	list(APPEND CMAKE_C_FLAGS " -Wall -Werror")
endif(UNIX)

include_directories(lua)
add_subdirectory(lua)

file(GLOB sources
	"*.c"
	"*.h")

add_executable(webserver ${sources})

target_link_libraries(webserver ws_lua)

if(WIN32)
	target_link_libraries(webserver ws2_32)
else(WIN32)
	target_link_libraries(webserver pthread m)
endif(WIN32)

if(WEBSERVER_WITH_VLD)
	add_definitions(-DWEBSERVER_WITH_VLD)
	target_link_libraries(webserver vld.lib)
endif(WEBSERVER_WITH_VLD)
