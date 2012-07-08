
file(GLOB sources
	"*.cpp"
	"*.hpp")

include_directories(..)

add_executable(test ${sources})
target_link_libraries(test compile run program)
