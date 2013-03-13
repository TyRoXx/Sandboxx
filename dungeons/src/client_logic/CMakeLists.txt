
file(GLOB src "*.hpp" "*.cpp")

add_library(client_logic ${src})
target_link_libraries(client_logic ${Boost_LIBRARIES} shared)
