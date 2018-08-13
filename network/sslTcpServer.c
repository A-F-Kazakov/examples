/**
 * @author   Казаков Андрей
 * @date     03.02.18.
 */

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

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
		perror("can't bind port");
		abort();
	}

	if(listen(sd, 10) != 0)
	{
		perror("Can't configure listening port");
		abort();
	}

	return sd;
}

int isRoot()
{
	if(getuid() != 0)
		return 0;
	return 1;
}

SSL_CTX* InitServerCTX()
{
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	const SSL_METHOD* method = DTLS_server_method();
	SSL_CTX* ctx = SSL_CTX_new(method);

	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}

	return ctx;
}

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

void ShowCerts(SSL* ssl)
{

	char* line;

	X509* cert = SSL_get_peer_certificate(ssl);

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
		printf("No certificates.\n");
}

void Servlet(SSL* ssl)
{
	const char* ServerResponse = "<Body><Name>Hello!</Name><year>2018</year><Author>Kazakov Andrei</Author></Body>";
	const char* cpValidMessage = "<Body><UserName>usr</UserName><Password>pwd</Password></Body>";

	if(SSL_accept(ssl) == -1)
		ERR_print_errors_fp(stderr);
	else
	{
		ShowCerts(ssl);

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

int main(int argc, char** argv)
{
	if(!isRoot())
	{
		printf("This program must be run as root/sudo user!!");
		exit(0);
	}

	if(argc != 2)
	{
		printf("Usage: %s <portnum>\n", argv[0]);
		exit(0);
	}

	SSL_library_init();

	char* portnum = argv[1];

	SSL_CTX* ctx = InitServerCTX();

	LoadCertificates(ctx, ("mycert.pem"), ("mycert.pem"));

	int server = OpenListener((uint16_t)atoi(portnum));

	while(1)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);

		int client = accept(server, (struct sockaddr*)&addr, &len);
		printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		SSL* ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		Servlet(ssl);
	}
	close(server);
	SSL_CTX_free(ctx);
}