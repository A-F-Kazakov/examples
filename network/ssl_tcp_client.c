#include "ssl_common.h"

int OpenConnection(const char* hostname, uint16_t port)
{
	int sd;
	struct hostent* host;

	if((host = gethostbyname(hostname)) == NULL)
	{
		perror(hostname);
		abort();
	}

	sd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family		= AF_INET;
	addr.sin_port			= htons(port);
	addr.sin_addr.s_addr = (in_addr_t)(*(long*)(host->h_addr));

	if(connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		close(sd);
		perror(hostname);
		abort();
	}
	return sd;
}

int main(int argc, char** argv, char** env)
{
	if(argc != 3)
	{
		printf("usage: %s <hostname> <port>\n", argv[0]);
		exit(0);
	}

	SSL_library_init();

	SSL_CTX* ctx = InitiateCtx(TLS_client_method());

	int server = OpenConnection(argv[1], (uint16_t)atoi(argv[2]));
	SSL* ssl	  = SSL_new(ctx);
	SSL_set_fd(ssl, server);

	int v = SSL_connect(ssl);
	if(v == -1)
		ERR_print_errors_fp(stderr);
	else
	{
		char acUsername[16]			  = {0};
		char acPassword[16]			  = {0};
		const char* cpRequestMessage = "<Body><UserName>%s</UserName><Password>%s</Password></Body>";

		printf("Enter the User Name : ");
		scanf("%s", acUsername);

		printf("\n\nEnter the Password : ");
		scanf("%s", acPassword);

		char acClientRequest[1024] = {0};

		sprintf(acClientRequest, cpRequestMessage, acUsername, acPassword);

		printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));

		ShowCertificates(ssl);
		SSL_write(ssl, acClientRequest, (int)strlen(acClientRequest));

		char buf[1024];

		int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);

		buf[bytes] = '\0';
		printf("Received: \"%s\"\n", buf);
		SSL_free(ssl);
	}

	close(server);
	SSL_CTX_free(ctx);
}
