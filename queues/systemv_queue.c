#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>

#define HANDLE_ERROR(X) if(X == -1){ printf("%s(%d)", strerror(errno), errno); return -1; }

struct message
{
	long mtype;
	char mtext[80];
};

/**
 * flags  int32
 * 00000000000000000000000		rwx		rwx		rwx
 * 	флаги						владелец создатель остальные
 * 	IPC_CREAT - создание очереди не обязателен при IPC_PRIVATE
 */

int main()
{
	key_t key = ftok("/tmp/msg.temp", 1);

	HANDLE_ERROR(key);

	int msqid = msgget(key, IPC_CREAT | 0666);

	HANDLE_ERROR(msqid);

	struct msqid_ds data;
	bzero(&data, sizeof(data));

	int out = msgctl(msqid, IPC_STAT, &data);

	HANDLE_ERROR(out);

	struct message msg;
	msg.mtype = 1;

	memcpy(msg.mtext, "Hello", 5);

	out = msgsnd(msqid, &msg, sizeof(struct message) - sizeof(long), 0);

	HANDLE_ERROR(out);

	printf("Sended: %d\n", out);

	out = (int)msgrcv(msqid, &msg, sizeof(msg), 1, 0);

//	std::cout << "Received: " << out << " : " << msg.mtext << std::endl;
	printf("Received(%d): %s\n", out, msg.mtext);

	out = msgctl(msqid, IPC_RMID, NULL);

	HANDLE_ERROR(out);
}
