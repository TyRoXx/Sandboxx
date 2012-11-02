cmake_minimum_required(VERSION 2.8)

if(UNIX)
	list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -Wall")
endif(UNIX)

project(vier_gewinnt)

file(GLOB files
	"*.hpp"
	"*.cpp"
	)

add_executable(vier_gewinnt ${files})
