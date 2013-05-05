project(state_machine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -pedantic")

add_executable(test test.cpp state_machine.hpp)
