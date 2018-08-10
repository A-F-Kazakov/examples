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

#define HANDLE_ERROR(X) if(X == -1){ printf("%s(%d)", strerror(errno), errno); return -1; }

int main(int argc, char** argv)
{
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	HANDLE_ERROR(listener);

//	std::set<int> sockets;

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int out = bind(listener, (struct sockaddr*)&addr, sizeof(addr));

	HANDLE_ERROR(out);

	out = fcntl(listener, F_GETFL, 0);

	HANDLE_ERROR(out);

	out = fcntl(listener, F_SETFL, res | O_NONBLOCK);

	HANDLE_ERROR(out);
	
	out = listen(listener, SOMAXCONN);

	HANDLE_ERROR(out);

	fd_set set;

	while(1)
	{
		FD_ZERO(&set);
		FD_SET(listener, &set);

//		for(auto&& it : sockets)
//			FD_SET(it, &set);

//		auto max = std::max(listener, *std::max_element(sockets.begin(), sockets.end()));

//		out = select(max + 1, &set, nullptr, nullptr, nullptr);

		HANDLE_ERROR(out);

		if(FD_ISSET(listener, &set))
		{
			int sock = accept(listener, NULL, NULL);

			int flags = fcntl(sock, F_GETFL, 0);

			out = fcntl(sock, F_SETFL, flags | O_NONBLOCK);

//			sockets.insert(sock);
		}
//		else
//			for(int it = sockets.begin(); it != sockets.end(); ++it)
//			{
//				if(FD_ISSET(*it, &set))
//				{
//					static char buff[1024];
//
//					ssize_t receive = recv(*it, buff, 1024, MSG_NOSIGNAL);
//
//					if(receive == 0 && errno != EAGAIN)
//					{
//						shutdown(*it, SHUT_RDWR);
//						close(*it);
//						sockets.erase(it);
//					}
//					else
//						if(receive != 0)
//							ssize_t sended = send(*it, buff, 1024, MSG_NOSIGNAL);
//				}
//			}
	}
}