#include <stdio.h>

#include "server.h"

int clean_and_exit(SOCKET *, int);

int main() {
  if (init_winsock()) {
    return EXIT_FAILURE;
  }

  SOCKET server_socket = create_server_socket();
  if (server_socket == INVALID_SOCKET) {
    server_teardown(server_socket);
    return EXIT_FAILURE;
  }

  while (1) {
    if (handle_connection(server_socket)) {
      printf("Something went wrong with the client socket...\n");
      printf("Continuing to listen for connections...\n");
    }
  }

  server_teardown(server_socket);
  return EXIT_SUCCESS;
}
