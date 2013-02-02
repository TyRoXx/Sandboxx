cmake_minimum_required(VERSION 2.8)

project(function)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wconversion -Wshadow -std=c++0x -g")

file(GLOB sources
	"*.hpp"
	"*.cpp")
add_executable(function ${sources})
