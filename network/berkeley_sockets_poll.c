/**
 * @author   Казаков Андрей 
 * @date     24.03.18.
 */

#include "common.h"

#include <poll.h>

#define POLL_SIZE 2048

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

	struct pollfd set[POLL_SIZE];
	bzero(&set, sizeof(set));

	set[0].fd = listener;
	set[0].events = POLLIN;

	int actualAmount = 1;

	while(1)
	{
		out = poll(set, (nfds_t)actualAmount, -1);

		HANDLE_ERROR(out);

		for(int i = 0; i < actualAmount && out; ++i)
			if(set[i].fd && (set[i].revents & POLLIN))
			{
				if(i)
				{
					static char buff[1024];

					ssize_t received = recv(set[i].fd, buff, 1024, MSG_NOSIGNAL);

					HANDLE_ERROR(received);

					if(received == 0 && errno != EAGAIN)
					{
						shutdown(set[i].fd, SHUT_RDWR);
						close(set[i].fd);

						set[i].fd = 0;
					}
					else
					{
						ssize_t sended = send(set[i].fd, buff, (size_t)received, MSG_NOSIGNAL);
						HANDLE_ERROR(sended);
					}
				}
				else
				{
					int sock = accept(listener, NULL, NULL);

					HANDLE_ERROR(sock);

					out = fcntl(sock, F_GETFL, 0);

					HANDLE_ERROR(out);

					out = fcntl(sock, F_SETFL, out | O_NONBLOCK);

					HANDLE_ERROR(out);

					int j = 0;
					while(1)
					{
						if(!set[j].fd)
						{
							set[j].fd = sock;
							set[j].events = POLLIN;
							break;
						}
						++j;
					}
				}
				--out;
			}
	}
}
