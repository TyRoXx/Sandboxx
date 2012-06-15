cmake_minimum_required(VERSION 2.8)

project(c_containers)

if(MSVC)
	list(APPEND CMAKE_C_FLAGS " -WX")
endif(MSVC)

add_executable(test main.c hash_map.c hash_map.h)
