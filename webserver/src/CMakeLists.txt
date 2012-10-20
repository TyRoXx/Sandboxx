cmake_minimum_required(VERSION 2.8)

project(webserver)

if(MSVC)
	set(WEBSERVER_WITH_VLD false CACHE BOOL "Visual Leak Detector, finds memory leaks with Visual C++ (vld.codeplex.com)")
	add_definitions("/wd4996") #'Funktion': wurde als veraltet deklariert
	add_definitions("/we4013") #behandelt Warnung "'..' undefined; assuming extern returning int" als Fehler
	add_definitions("/WX")
endif(MSVC)

if(UNIX)
	list(APPEND CMAKE_C_FLAGS " -Wall -Werror -std=gnu99") #consider -pedantic
endif(UNIX)

include_directories(.)
include_directories(lua)

add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(sub_directory)
add_subdirectory(file_system)
add_subdirectory(lua)
add_subdirectory(lua_script)
add_subdirectory(test)
add_subdirectory(webserver)
add_subdirectory(empty_plugin)
