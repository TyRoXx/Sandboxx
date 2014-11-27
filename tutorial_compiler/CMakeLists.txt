project(tutorial_compiler)
cmake_minimum_required(VERSION 2.8)

if(UNIX)
	add_definitions("-std=c++0x -Wall -Wextra -Wconversion -pedantic")
endif()

add_executable(tutorial_compiler compiler.cpp)
