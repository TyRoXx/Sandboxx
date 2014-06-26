cmake_minimum_required(VERSION 2.8)
project(newlanguage)

if(UNIX)
	add_definitions("-Wall -Wextra -Wconversion -pedantic -std=c++0x")

	find_package(Boost REQUIRED unit_test_framework system)
	include_directories(SYSTEM ${Boost_INCLUDE_DIR})
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

if(MSVC)
	add_definitions("/MP")
	add_definitions("/W4")
endif()

set(SILICIUM_INCLUDE_DIR CACHE FILEPATH "")
if(NOT SILICIUM_INCLUDE_DIR)
	message(FATAL_ERROR "Silicium required")
endif()
include_directories(SYSTEM ${SILICIUM_INCLUDE_DIR})

include_directories(.)
file(GLOB headers "ast/*.hpp" "semantic/*.hpp" "*.hpp")
add_executable(test ${headers} test.cpp)
target_link_libraries(test ${Boost_LIBRARIES})
