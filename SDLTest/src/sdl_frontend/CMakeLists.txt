file(GLOB sources "*.h" "*.c")

add_library(sdl_frontend ${sources})
target_link_libraries(sdl_frontend frontend)

