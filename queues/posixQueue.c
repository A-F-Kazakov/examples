#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>

#define HANDLE_ERROR(X) if(X == -1){ printf("%s(%d)", strerror(errno), errno); return -1; }

int main()
{
	const char* name = "/queue";

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 99;
	attr.mq_curmsgs = 0;

	int queue = mq_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);

	HANDLE_ERROR(queue);

	int out = mq_getattr(queue, &attr);

	HANDLE_ERROR(out);

	out = mq_send(queue, "hello world", 11, 32);

	HANDLE_ERROR(out);

	char buff[100];

	out = (int)mq_receive(queue, buff, 100, NULL);

	HANDLE_ERROR(out);

	buff[out] = '\0';

	printf("Received from queue %d bytes: %s", out, buff);

	out = mq_close(queue);

	HANDLE_ERROR(out);

	out = mq_unlink(name);

	HANDLE_ERROR(out);
}