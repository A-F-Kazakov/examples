set(HEAD_NAME ${PROJECT_NAME}-system)

if(WIN32)
	add_executable(${HEAD_NAME}-window WIN32 window_winapi.c)
	target_compile_definitions(${HEAD_NAME}-window PUBLIC UNICODE)
else()
	CHECK_INCLUDE_FILE(X11/Xlib.h X11)

	if(X11)
		message(STATUS "X11 window example available")

		add_executable(${HEAD_NAME}-window window_x11.c)
		target_link_libraries(${HEAD_NAME}-window X11)
	endif()
endif()

if(UNIX AND NOT APPPLE)
	message(STATUS "Linux module examples available")

	set(KERNEL_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/build")

	set(BUILD_CMD ${CMAKE_MAKE_PROGRAM} -C ${KERNEL_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules)

	add_custom_command(OUTPUT lkm
							 COMMAND ${BUILD_CMD}
							 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
							 DEPENDS
								linux_kernel_module_simple.c
								linux_kernel_module_extended.c
							 COMMENT "Building kernel module..."
	)

	add_custom_target(${HEAD_NAME}-simple-driver ALL DEPENDS ${lkm})
endif()
