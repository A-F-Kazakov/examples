
if(UNIX AND NOT APPPLE)
	set(KERNEL_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/build")

	set(BUILD_CMD ${CMAKE_MAKE_PROGRAM} -C ${KERNEL_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules)

	add_custom_command(OUTPUT lkm
							 COMMAND ${BUILD_CMD}
							 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
							 DEPENDS
							 linux_kernel_module_simple.c
							 linux_kernel_module_extended.c

							 COMMENT "Building kernel module...")

	add_custom_target(simple_driver ALL DEPENDS ${lkm})
endif()
