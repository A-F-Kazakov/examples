cmake_minimum_required(VERSION 3.16)

project(examples LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE	OFF)

include(CheckIncludeFile)

add_subdirectory(network)
add_subdirectory(queues)
add_subdirectory(system)
