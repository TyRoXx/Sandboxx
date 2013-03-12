
file(GLOB src "*.hpp" "*.cpp")

add_library(sdlpp ${src})
target_link_libraries(sdlpp ${SDL_LIBRARY} ${SDLTTF_LIBRARY})
