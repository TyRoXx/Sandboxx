cmake_minimum_required(VERSION 2.8)

project(webserver)

if(MSVC)
	set(WEBSERVER_WITH_VLD false CACHE BOOL "Visual Leak Detector, finds memory leaks with Visual C++ (vld.codeplex.com)")
	add_definitions("/wd4996") #'Funktion': wurde als veraltet deklariert
endif(MSVC)

if(UNIX)
	list(APPEND CMAKE_C_FLAGS " -Wall -Werror")
endif(UNIX)

include_directories(.)
include_directories(lua)

add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(file_system)
add_subdirectory(lua)
add_subdirectory(lua_script)
add_subdirectory(test)
add_subdirectory(webserver)
