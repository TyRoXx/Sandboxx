project(signals)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_C_FLAGS "-ansi -Wall -Werror -pedantic -g")

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

