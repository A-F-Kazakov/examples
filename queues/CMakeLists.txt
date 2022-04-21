
CHECK_INCLUDE_FILE(mqueue.h MQUEUE)

if(MQUEUE)
	add_executable(posix_queue	posix_queue.c)
	target_link_libraries(posix_queue rt)
endif()

CHECK_INCLUDE_FILE(sys/msg.h SYSQUEUE)

if(SYSQUEUE)
	add_executable(systemv_queue	systemv_queue.c)
	target_link_libraries(systemv_queue rt)
endif()
