set(HEAD_NAME "echo_server")

add_executable(${HEAD_NAME}_select	berkeley_sockets_select.c)
add_executable(${HEAD_NAME}_poll		berkeley_sockets_poll.c)
add_executable(${HEAD_NAME}_epoll	berkeley_sockets_epoll.c)

if(BSD)
	add_executable(${HEAD_NAME}_kqueue	berkeley_sockets_kqueue.c)
endif()

add_executable(sslEchoServer sslTcpServer.c)
target_link_libraries(sslEchoServer PUBLIC ssl crypto)

add_executable(sslEchoClient sslTcpClient.c)
target_link_libraries(sslEchoClient PUBLIC ssl crypto)

add_executable(bluetooth_server unix_bluetooth_server.c)
add_executable(bluetooth_client unix_bluetooth_client.c)
