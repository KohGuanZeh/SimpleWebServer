#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>

typedef enum { OK = 0, CLIENT_ERROR = 1, SERVER_ERROR = 2 } ConnectionStatus;

int init_winsock();
void server_teardown(SOCKET);

SOCKET create_server_socket();

int handle_connection(SOCKET);

#endif