
file(GLOB sources
	"*.cpp"
	"*.hpp")

include_directories(..)

add_executable(cmdline ${sources})
target_link_libraries(cmdline compile run program)
