file(GLOB sources
	"*.c"
	"*.h")

add_library(lua_script ${sources})
