cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED filesystem system iostreams program_options)
include_directories(${Boost_INCLUDEDIR})

add_definitions("-Wall -Wextra -pedantic -std=c++0x")

add_executable(nofs nofs.cpp)
target_link_libraries(nofs ${Boost_LIBRARIES})
