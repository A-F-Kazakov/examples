set(HEAD_NAME "echo_server")

CHECK_INCLUDE_FILE(sys/socket.h SOCKET)

if(SOCKET)
	add_executable(${HEAD_NAME}_select	berkeley_sockets_select.c)
	add_executable(${HEAD_NAME}_poll		berkeley_sockets_poll.c)
	add_executable(${HEAD_NAME}_epoll	berkeley_sockets_epoll.c)
endif()

CHECK_INCLUDE_FILE(sys/event.h K_QUEUE)

if(K_QUEUE)
	add_executable(${HEAD_NAME}_kqueue	berkeley_sockets_kqueue.c)
endif()

#

CHECK_INCLUDE_FILE(openssl/ssl.h SSL)

if(SSL)
	add_executable(sslEchoServer sslTcpServer.c)
	target_link_libraries(sslEchoServer PUBLIC ssl crypto)

	add_executable(sslEchoClient sslTcpClient.c)
	target_link_libraries(sslEchoClient PUBLIC ssl crypto)
endif()

#

CHECK_INCLUDE_FILE(bluetooth/bluetooth.h BLUETOOTH)

if(BLUETOOTH)
	add_executable(bluetooth_server unix_bluetooth_server.c)
	target_link_libraries(bluetooth_server PUBLIC bluetooth)
	 
	add_executable(bluetooth_client unix_bluetooth_client.c)
	target_link_libraries(bluetooth_client PUBLIC bluetooth)
endif()
