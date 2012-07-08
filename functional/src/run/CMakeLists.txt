
file(GLOB sources
	"*.cpp"
	"*.hpp")

include_directories(..)

add_library(run ${sources})
