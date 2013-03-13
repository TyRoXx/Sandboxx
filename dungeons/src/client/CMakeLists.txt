file(GLOB src "*.hpp" "*.cpp")

add_executable(client ${src})
target_link_libraries(client sdl_display)
