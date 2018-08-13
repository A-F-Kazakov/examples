/**
 * @author   Казаков Андрей
 * @date     01.02.18.
 */

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int OpenConnection(const char* hostname, uint16_t port)
{
	int sd;
	struct hostent* host;

	if((host = gethostbyname(hostname)) == NULL)
	{
		perror(hostname);
		abort();
	}

	sd       = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = (in_addr_t)(*(long*)(host->h_addr));


	if(connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		close(sd);
		perror(hostname);
		abort();
	}
	return sd;
}

SSL_CTX* InitCTX()
{
	const SSL_METHOD* method;
	SSL_CTX* ctx;

	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = DTLS_client_method();
	ctx = SSL_CTX_new(method);

	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	return ctx;
}

void ShowCerts(SSL* ssl)
{
	X509* cert;
	char* line;

	cert = SSL_get_peer_certificate(ssl);

	if(cert != NULL)
	{
		printf("Server certificates:\n");

		line = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
		printf("Subject: %s\n", line);
		free(line);

		line = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
		printf("Issuer: %s\n", line);
		free(line);

		X509_free(cert);
	}
	else
		printf("Info: No client certificates configured.\n");
}

int main(int argc, char** argv, char** env)
{
	if(argc != 3)
	{
		printf("usage: %s <hostname> <port>\n", argv[0]);
		exit(0);
	}

	char* hostname, * portnum;

	SSL_library_init();
	hostname = argv[1];
	portnum = argv[2];

	SSL_CTX* ctx;
	int server;
	SSL* ssl;

	ctx = InitCTX();
	server = OpenConnection(hostname, (uint16_t)atoi(portnum));
	ssl = SSL_new(ctx);      /* create new SSL connection state */
	SSL_set_fd(ssl, server);    /* attach the socket descriptor */

	if(SSL_connect(ssl) == -1)   /* perform the connection */
		ERR_print_errors_fp(stderr);
	else
	{
		char acUsername[16] = {0};
		char acPassword[16] = {0};
		const char* cpRequestMessage = "<Body><UserName>%s</UserName><Password>%s</Password></Body>";

		printf("Enter the User Name : ");
		scanf("%s", acUsername);

		printf("\n\nEnter the Password : ");
		scanf("%s", acPassword);

		char acClientRequest[1024] = {0};

		sprintf(acClientRequest, cpRequestMessage, acUsername, acPassword);

		printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));

		ShowCerts(ssl);
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