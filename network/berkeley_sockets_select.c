/**
 * @author   Казаков Андрей 
 * @date     24.03.18.
 */

#include "common.h"

int main(int argc, char** argv)
{
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	HANDLE_ERROR(listener);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	int out = bind(listener, (struct sockaddr*)&addr, sizeof(addr));

	HANDLE_ERROR(out);

	out = fcntl(listener, F_GETFL, 0);

	HANDLE_ERROR(out);

	out = fcntl(listener, F_SETFL, out | O_NONBLOCK);

	HANDLE_ERROR(out);
	
	out = listen(listener, SOMAXCONN);

	HANDLE_ERROR(out);

	fd_set set;
	int sockets[128];
	bzero(sockets, 128 * 4);

	sockets[0] = listener;

	while(1)
	{
		FD_ZERO(&set);
		FD_SET(listener, &set);

		int max = 0;
		for(int i = 0; i < 128; ++i)
		{
			if(max < sockets[i])
				max = sockets[i];
			if(sockets[i])
				FD_SET(sockets[i], &set);
		}

		out = select(max + 1, &set, NULL, NULL, NULL);

		HANDLE_ERROR(out);

		if(FD_ISSET(listener, &set))
		{
			int sock = accept(listener, NULL, NULL);

			HANDLE_ERROR(sock);

			out = fcntl(sock, F_GETFL, 0);

			HANDLE_ERROR(out);

			out = fcntl(sock, F_SETFL, out | O_NONBLOCK);

			HANDLE_ERROR(out);

			for(int i = 0; i < 128; ++i)
			if(!sockets[i])
			{
				sockets[i] = sock;
				break;
			}
		}
		else
			for(int i = 0; i < 128; ++i)
			{
				if(FD_ISSET(sockets[i], &set))
				{
					static char buff[1024];

					ssize_t received = recv(sockets[i], buff, 1024, MSG_NOSIGNAL);

					HANDLE_ERROR(received);

					if(received == 0 && errno != EAGAIN)
					{
						shutdown(sockets[i], SHUT_RDWR);
						close(sockets[i]);
						sockets[i] = 0;
					}
					else
					{
						ssize_t sended = send(sockets[i], buff, (size_t)received, MSG_NOSIGNAL);
						HANDLE_ERROR(sended);
					}
				}
			}
	}
}