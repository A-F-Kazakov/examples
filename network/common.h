/**
 * @author   Казаков Андрей 
 * @date     13.08.18.
 */

#ifndef UNIX_EXAMPLES_COMMON_H
#define UNIX_EXAMPLES_COMMON_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define HANDLE_ERROR(X) if(X == -1){ printf("%s(%d)", strerror(errno), errno); return -1; }

#endif //UNIX_EXAMPLES_COMMON_H
