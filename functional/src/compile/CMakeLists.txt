
file(GLOB sources
	"*.cpp"
	"*.hpp")

include_directories(..)

add_library(compile ${sources})
