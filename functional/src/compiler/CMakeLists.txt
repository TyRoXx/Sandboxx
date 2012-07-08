
file(GLOB sources
	"*.cpp"
	"*.hpp")

include_directories(..)

add_executable(compiler ${sources})
target_link_libraries(compiler compile)