/**
 * @author   Казаков Андрей
 * @date     27.08.18.
 *
 * openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout server.key -out server.crt
 */

#ifndef UNIX_EXAMPLES_SSLCOMMON_H
#define UNIX_EXAMPLES_SSLCOMMON_H

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* InitiateCtx(const SSL_METHOD* method)
{
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	SSL_CTX* ctx = SSL_CTX_new(method);

	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}

	return ctx;
}

void ShowCertificates(SSL* ssl)
{
	X509* cert = SSL_get_peer_certificate(ssl);

	if(cert != NULL)
	{
		printf("Certificates information:\n");

		char* line = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
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

int isRoot()
{
	if(getuid() != 0)
		return 0;
	return 1;
}

#endif //UNIX_EXAMPLES_SSLCOMMON_H
