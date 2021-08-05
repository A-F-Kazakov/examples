
add_executable(posixQueue	posixQueue.c)
target_link_libraries(posixQueue rt)

add_executable(systemVQueue	sysV_queue.c)
target_link_libraries(systemVQueue rt)
