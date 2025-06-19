set(HEAD_NAME ${PROJECT_NAME}-network)

CHECK_INCLUDE_FILE(sys/socket.h SOCKET)

if(SOCKET)
	message(STATUS "socket examples available")

	set(TARGET_NAME ${HEAD_NAME}-echo-server)

	add_executable(${TARGET_NAME}-select	berkeley_sockets_select.c)
	add_executable(${TARGET_NAME}-poll		berkeley_sockets_poll.c)
	add_executable(${TARGET_NAME}-epoll		berkeley_sockets_epoll.c)
endif()

CHECK_INCLUDE_FILE(sys/event.h K_QUEUE)

if(K_QUEUE)
	add_executable(${TARGET_NAME}-kqueue berkeley_sockets_kqueue.c)
endif()

#

CHECK_INCLUDE_FILE(openssl/ssl.h SSL)

if(SSL)
	message(STATUS "ssl examples available")

	set(TARGET_NAME ${HEAD_NAME}-ssl-server)

	add_executable(${TARGET_NAME} ssl_tcp_server.c)
	target_link_libraries(${TARGET_NAME} PUBLIC ssl crypto)

	set(TARGET_NAME ${HEAD_NAME}-ssl-client)

	add_executable(${TARGET_NAME} ssl_tcp_client.c)
	target_link_libraries(${TARGET_NAME} PUBLIC ssl crypto)
endif()

#

CHECK_INCLUDE_FILE(bluetooth/bluetooth.h BLUETOOTH)

if(BLUETOOTH)
	message(STATUS "bluetooth examples available")

	set(TARGET_NAME ${HEAD_NAME}-bluetooth-server)

	add_executable(${TARGET_NAME} unix_bluetooth_server.c)
	target_link_libraries(${TARGET_NAME} PUBLIC bluetooth)

	set(TARGET_NAME ${HEAD_NAME}-bluetooth-client)

	add_executable(${TARGET_NAME} unix_bluetooth_client.c)
	target_link_libraries(${TARGET_NAME} PUBLIC bluetooth)
endif()

find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
	pkg_check_modules(ssh IMPORTED_TARGET libssh)

	if(ssh_FOUND)
		message(STATUS "ssh examples available")

		set(TARGET_NAME ${HEAD_NAME}-ssh-client)

		add_executable(${TARGET_NAME} ssh.c)
		target_link_libraries(${TARGET_NAME} PUBLIC PkgConfig::ssh)
	endif()
endif()
