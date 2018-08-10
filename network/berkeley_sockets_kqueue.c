/**
 * @author   Казаков Андрей 
 * @date     24.03.18.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>

#define HANDLE_ERROR(X) if(X == -1){ printf("%s(%d)", strerror(errno), errno); return -1; }

int main(int argc, char** argv)
{
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	HANDLE_ERROR(listener);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int aut = bind(listener, (struct sockaddr*)&addr, sizeof(addr));

	HANDLE_ERROR(out);

	out = fcntl(listener, F_GETFL, 0);

	HANDLE_ERROR(out);

	out = fcntl(listener, F_SETFL, res | O_NONBLOCK | SO_REUSEADDR | SO_REUSEPORT);

	HANDLE_ERROR(out);
	
	out = listen(listener, SOMAXCONN);

	HANDLE_ERROR(out);

	int queue = kqueue();

	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, listener, EVFILT_READ, EV_ADD, 0, 0, 0);

	out = kevent(queue, &event, 1, NULL, 0, NULL);

	HANDLE_ERROR(out);

	while(1)
	{
		bzero(&event, sizeof(event));
		out = kevent(queue, NULL, 0, &event, 1, NULL);

		HANDLE_ERROR(out);

		if(event.filter == EVFILT_READ)
		{
			if(event.ident == listener)
			{
				int sock = accept(listener, NULL, NULL);

				HANDLE_ERROR(sock);

				out = fcntl(sock, F_GETFL, 0);

				HANDLE_ERROR(out);

				out = fcntl(sock, F_SETFL, out | O_NONBLOCK);

				HANDLE_ERROR(out);

				sockets.insert(sock);

				bzero(&event, sizeof(event));
				EV_SET(&event, sock, EVFILT_READ, EV_ADD, 0, 0, 0);
				out = kevent(queue, &kevent, 1, NULL, 0, NULL);

				HANDLE_ERROR(out);
			}
			else
			{
				static char buff[1024];

				ssize_t received = recv(ev.data.fd, buff, 1024, MSG_NOSIGNAL);

				HANDLE_ERROR(received);

				if(received == 0 && errno != EAGAIN)
				{
					shutdown(event.ident, SHUT_RDWR);
					close(event.ident);
					sockets.erase(event.ident);
				}
				else
				{
					ssize_t sended = send(ev.data.fd, buff, received, MSG_NOSIGNAL);
					HANDLE_ERROR(sended);
				}
			}
		}
	}
}