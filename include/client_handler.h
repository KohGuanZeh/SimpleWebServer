#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <winsock2.h>

#define MAX_BUFFER_SIZE 512

int handle_client(SOCKET);

#endif