cmake_minimum_required(VERSION 2.8)

project(skyfall)

add_definitions("-DSFML_STATIC")

file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(skyfall ${files})

target_link_libraries(skyfall
	debug sfml-graphics-s-d
	debug sfml-window-s-d
	debug sfml-system-s-d
	optimized sfml-graphics-s
	optimized sfml-window-s
	optimized sfml-system-s
	)
