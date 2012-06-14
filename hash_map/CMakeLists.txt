cmake_minimum_required(VERSION 2.8)

project(hash_map)

if(MSVC)
	list(APPEND CMAKE_C_FLAGS " -WX")
endif(MSVC)

add_executable(hash_map main.c hash_map.c hash_map.h)
