project(SDLTest)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Jansson REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(${JANSSON_INCLUDE_DIR})
include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDLTTF_INCLUDE_DIR})

if(UNIX OR MINGW)
	set(CMAKE_C_FLAGS "-Wall -Wextra -Wconversion -Werror -pedantic -Wconversion")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu89")
endif()

if(MSVC)
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

include_directories(".")

add_subdirectory(base)
add_subdirectory(frontend)
add_subdirectory(sdl_frontend)
add_subdirectory(game)
