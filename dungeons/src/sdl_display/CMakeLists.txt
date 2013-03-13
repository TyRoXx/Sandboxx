file(GLOB src "*.hpp" "*.cpp")

add_library(sdl_display ${src})
target_link_libraries(sdl_display sdlpp client_logic)
