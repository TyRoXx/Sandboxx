
file(GLOB src "*.hpp" "*.cpp")

add_executable(client ${src})
target_link_libraries(client sdlpp ${Boost_LIBRARIES} shared)

