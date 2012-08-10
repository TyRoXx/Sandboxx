project(c_wrapper)

add_library(map map.h map.cpp)

add_executable(test main.c)
target_link_libraries(test map)
