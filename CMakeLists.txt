cmake_minimum_required(VERSION 2.9)
project(unix_examples)

set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE	OFF)

add_subdirectory(network)
add_subdirectory(queues)
add_subdirectory(system)