project(dungeons)
cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR})

find_package(Boost REQUIRED system)
include_directories(${Boost_INCLUDE_DIR})

if (UNIX OR MINGW)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wconversion -pedantic -std=c++0x")
endif()

if (MSVC)
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

include_directories(".")

add_subdirectory(shared)
add_subdirectory(sdlpp)
add_subdirectory(client_logic)
add_subdirectory(sdl_display)
add_subdirectory(client)
add_subdirectory(server)
