
CHECK_INCLUDE_FILE(mqueue.h MQUEUE)

if(MQUEUE)
	message(STATUS "posix queue example available")

	set(TARGET_NAME ${PROJECT_NAME}-queue-posix)

	add_executable(${TARGET_NAME}	posix_queue.c)
	target_link_libraries(${TARGET_NAME} rt)
endif()

CHECK_INCLUDE_FILE(sys/msg.h SYSQUEUE)

if(SYSQUEUE)
	message(STATUS "system v queue example available")

	set(TARGET_NAME ${PROJECT_NAME}-queue-sysv)

	add_executable(${TARGET_NAME}	systemv_queue.c)
	target_link_libraries(${TARGET_NAME} rt)
endif()
