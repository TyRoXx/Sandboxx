project(SDLTest)
cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
	list(APPEND CMAKE_C_FLAGS "-ansi -Wall -Wextra -Wconversion -pedantic -g")
endif()

if (MSVC)
	add_definitions("/D_SCL_SECURE_NO_WARNINGS")
	#list(APPEND CMAKE_C_FLAGS "/wd4996") #This function or variable may be unsafe.
endif()

file(GLOB sources "*.h" "*.c")

add_executable(${PROJECT_NAME} ${sources})

if (MINGW)
	target_link_libraries(${PROJECT_NAME} "-lmingw32")
	target_link_libraries(${PROJECT_NAME} ${SDLMAIN_LIBRARY})
endif()

target_link_libraries(${PROJECT_NAME} ${SDLTTF_LIBRARY} ${SDL_LIBRARY})

if (UNIX)
	target_link_libraries(${PROJECT_NAME} "-lm")
endif()
