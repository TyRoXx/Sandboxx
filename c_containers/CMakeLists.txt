cmake_minimum_required(VERSION 2.8)

project(c_containers)

if(MSVC)
	list(APPEND CMAKE_C_FLAGS " -WX")
endif(MSVC)

file(GLOB sources
	"*.h"
	"*.c")

add_executable(test ${sources})
