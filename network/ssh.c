#include <libssh/libssh.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if(argc < 4)
    {
		printf("<hostname> <username> <password>");
		 return -1;
    }

    const char* hostname = argv[1];
    const char* username = argv[2];
    const char* password = argv[3];

	 ssh_session my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
    {
        printf("Error: Could not create SSH session.");
        return -1;
    }

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, username);

    int rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        printf("Error: Unable to connect to %s. %s", hostname, ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        printf("Error: Authentication failed. %s", ssh_get_error(my_ssh_session));
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    ssh_channel channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL)
    {
		printf("Error: Could not open channel.");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        printf("Error: Could not open session channel. %s", ssh_get_error(my_ssh_session));
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_channel_request_exec(channel, "ls -l");
    if (rc != SSH_OK)
    {
        printf("Error: Could not execute command. %s", ssh_get_error(my_ssh_session));
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    char buffer[256];
    int nbytes;
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0)
		 printf("%.*s", nbytes, buffer);

    if (nbytes < 0)
        printf("Error: Reading from channel failed.");

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);

    return 0;
}
