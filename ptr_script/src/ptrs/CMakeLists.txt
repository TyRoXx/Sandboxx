
file(GLOB files "*.hpp" "*.cpp")

add_executable(ptrs ${files})
target_link_libraries(ptrs package common)
