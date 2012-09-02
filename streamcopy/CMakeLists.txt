cmake_minimum_required(VERSION 2.8)

project(streamcopy)

file(GLOB files "*.hpp" "*.cpp")

add_executable(streamcopy ${files})
