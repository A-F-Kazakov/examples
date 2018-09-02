/**
 * @author   Казаков Андрей
 * @date     03.02.18.
 */

#include <signal.h>

#include "sslCommon.h"

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
	if(SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}

	if(SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}

	if(!SSL_CTX_check_private_key(ctx))
	{
		fprintf(stderr, "Private key does not match the public certificate\n");
		abort();
	}
}

int OpenListener(uint16_t port)
{
	int sd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		perror("Can't bind port");
		abort();
	}

	if(listen(sd, 10) != 0)
	{
		perror("Can't configure listening port");
		abort();
	}

	return sd;
}

void Servlet(SSL* ssl)
{
	const char* ServerResponse = "<Body><Name>Hello!</Name><year>2018</year><Author>@username</Author></Body>";
	const char* cpValidMessage = "<Body><UserName>usr</UserName><Password>pwd</Password></Body>";

	if(SSL_accept(ssl) == -1)
		ERR_print_errors_fp(stderr);
	else
	{
		ShowCertificates(ssl);

		char buf[1024] = {0};
		int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);
		buf[bytes] = '\0';

		printf("Client msg: \"%s\"\n", buf);

		if(bytes > 0)
		{
			if(strcmp(cpValidMessage, buf) == 0)
				SSL_write(ssl, ServerResponse, (int)(strlen(ServerResponse)));
			else
				SSL_write(ssl, "Invalid Message", (int)(strlen("Invalid Message")));
		}
		else
			ERR_print_errors_fp(stderr);

	}
	int sd = SSL_get_fd(ssl);
	SSL_free(ssl);
	close(sd);
}

int* sockPtr = NULL;

void signalHandler(int sig)
{
	close(*sockPtr);
}

int main(int argc, char** argv)
{
	if(!isRoot())
	{
		printf("This program must be run as root/sudo user!!\n");
		return 0;
	}

	if(argc != 4)
	{
		printf("Usage: %s <port> <cert> <key>\n", argv[0]);
		exit(0);
	}

	printf("Starting server on port %s\n", argv[1]);
	fflush(stdout);

	signal(SIGINT, signalHandler);

	SSL_library_init();

	SSL_CTX* ctx = InitiateCtx(DTLS_server_method());

	LoadCertificates(ctx, (argv[2]), (argv[3]));

	int server = OpenListener((uint16_t)atoi(argv[1]));

	sockPtr = &server;

	while(1)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);

		int client = accept(server, (struct sockaddr*)&addr, &len);

		if(client == -1 && errno == 9)
			break;

		printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		SSL* ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		Servlet(ssl);
	}

	printf("\b\bStopping server\n");

	close(server);
	SSL_CTX_free(ctx);
}