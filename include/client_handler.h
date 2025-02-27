#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <winsock2.h>

#define MAX_BUFFER_SIZE 8192

int handle_client(SOCKET);

#endif