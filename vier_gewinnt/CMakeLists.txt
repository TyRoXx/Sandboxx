cmake_minimum_required(VERSION 2.8)

project(vier_gewinnt)

file(GLOB files
	"*.hpp"
	"*.cpp"
	)

add_executable(vier_gewinnt ${files})
