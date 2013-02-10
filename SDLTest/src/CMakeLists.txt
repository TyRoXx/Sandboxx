project(SDLTest)
cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR})

if (UNIX OR MINGW)
	set(CMAKE_C_FLAGS "-Wall -Wextra -Wconversion -Wshadow -pedantic -g")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif()

if (MSVC)
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

include_directories(".")

add_subdirectory(base)
add_subdirectory(sdl_frontend)
add_subdirectory(game)
