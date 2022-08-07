cmake_minimum_required(VERSION 3.16)

project(examples LANGUAGES C)

set(CMAKE_C_STANDARD		11)
set(CMAKE_CXX_STANDARD		17)
set(CMAKE_VERBOSE_MAKEFILE	OFF)

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(CheckIncludeFile)

add_subdirectory(network)
add_subdirectory(queues)
add_subdirectory(system)
add_subdirectory(patterns)
