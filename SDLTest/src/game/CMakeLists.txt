file(GLOB sources "*.h" "*.c")

add_executable(game ${sources})

target_link_libraries(game sdl_frontend base)

if(MINGW)
	target_link_libraries(game "-lmingw32")
	target_link_libraries(game ${SDLMAIN_LIBRARY})
endif()

target_link_libraries(game ${SDLTTF_LIBRARY} ${SDL_LIBRARY} ${JANSSON_LIBRARY})

if(UNIX)
	target_link_libraries(game "-lm")
endif()
