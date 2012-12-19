project(SDLTest)
cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)

include_directories(${SDL_INCLUDE_DIR})

list(APPEND CMAKE_C_FLAGS "-ansi -Wall -Wextra -pedantic -g")

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

if (MINGW)
	target_link_libraries(${PROJECT_NAME} "-lmingw32")
endif()

target_link_libraries(${PROJECT_NAME} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY} "-lm")
