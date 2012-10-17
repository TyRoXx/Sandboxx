file(GLOB sources
	"*.c"
	"*.h")

add_library(empty_plugin SHARED ${sources})
