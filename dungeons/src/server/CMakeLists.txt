
file(GLOB src "*.hpp" "*.cpp")

add_executable(server ${src})
target_link_libraries(server ${Boost_LIBRARIES} shared)
