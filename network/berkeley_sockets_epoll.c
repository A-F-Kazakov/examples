/**
 * @author   Казаков Андрей 
 * @date     24.03.18.
 */

#include "common.h"

#include <sys/epoll.h>

#define MAX_EVENTS 2048

int main(int argc, char** argv)
{
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	HANDLE_ERROR(listener);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int out = bind(listener, (struct sockaddr*)&addr, sizeof(addr));

	HANDLE_ERROR(out);

	out = fcntl(listener, F_GETFL, 0);

	HANDLE_ERROR(out);

	out = fcntl(listener, F_SETFL, out | O_NONBLOCK | SO_REUSEADDR | SO_REUSEPORT);

	HANDLE_ERROR(out);
	
	out = listen(listener, SOMAXCONN);

	HANDLE_ERROR(out);

	int epoll = epoll_create1(0);

	HANDLE_ERROR(epoll);

	struct epoll_event event;
	bzero(&event, sizeof(event));

	event.data.fd = listener;
	event.events = EPOLLIN;

	out = epoll_ctl(epoll, EPOLL_CTL_ADD, listener, &event);

	HANDLE_ERROR(out);

	struct epoll_event events[MAX_EVENTS];

	while(1)
	{
		int eventsCount = epoll_wait(epoll, events, MAX_EVENTS, -1);

		HANDLE_ERROR(eventsCount);

		for(int i = 0; i < eventsCount; ++i)
			if(events[i].data.fd == listener)
			{
				int sock = accept(listener, NULL, NULL);

				HANDLE_ERROR(sock);

				out = fcntl(sock, F_GETFL, 0);

				HANDLE_ERROR(out);

				out = fcntl(sock, F_SETFL, out | O_NONBLOCK);

				HANDLE_ERROR(out);

				event.data.fd = sock;
				out = epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &event);

				HANDLE_ERROR(out);
			}
			else
			{
				static char buff[1024];

				ssize_t received = recv(events[i].data.fd, buff, 1024, MSG_NOSIGNAL);

				HANDLE_ERROR(received);

				if(received == 0 && errno != EAGAIN)
				{
					shutdown(events[i].data.fd, SHUT_RDWR);
					close(events[i].data.fd);
				}
				else
				{
					ssize_t sended = send(events[i].data.fd, buff, (size_t)received, MSG_NOSIGNAL);

					HANDLE_ERROR(sended);
				}
			}
	}
}
