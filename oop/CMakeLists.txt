cmake_minimum_required(VERSION 2.8)

project(coop)

file(GLOB files "*.h" "*.c")

add_executable(coop ${files})
