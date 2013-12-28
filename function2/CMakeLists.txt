project(function2)

if(UNIX)
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -pedantic -Wconversion")
endif()

add_executable(test test.cpp function.hpp function2.hpp)
