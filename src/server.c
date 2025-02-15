#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>

#include "client_handler.h"

/**
 * @brief Initializes Winsock library with WSAStartup.
 *
 * @return Returns 0 on success, 1 on failure.
 */
int init_winsock() {
  WSADATA wsaData;
  int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

  if (wsaResult != 0) {
    printf("WSAStartup failed: %d\n", wsaResult);
    return 1;
  }

  return 0;
}

/**
 * @brief Initializes the server listening socket.
 *
 * @return Returns newly created server listening socket, on success,
 * `INVALID_SOCKET` on failure.
 */
SOCKET create_server_socket() {
  char const *const HTTP_PORT = "80";

  struct addrinfo *addr_info = NULL;

  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  int wsaResult = getaddrinfo(NULL, HTTP_PORT, &hints, &addr_info);
  if (wsaResult != 0) {
    printf("getaddrinfo failed: %d\n", wsaResult);
    return INVALID_SOCKET;
  }

  // Create socket for server to listen for client connections
  SOCKET server_socket = INVALID_SOCKET;
  server_socket = socket(addr_info->ai_family, addr_info->ai_socktype,
                         addr_info->ai_protocol);

  if (server_socket == INVALID_SOCKET) {
    printf("Error at socket(): %d\n", WSAGetLastError());
    freeaddrinfo(addr_info);
    return INVALID_SOCKET;
  }

  // Setup the TCP listening socket
  wsaResult =
      bind(server_socket, addr_info->ai_addr, (int)addr_info->ai_addrlen);
  if (wsaResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(addr_info);
    closesocket(server_socket);
    return INVALID_SOCKET;
  }

  // Free memory allocated by getaddrinfo()
  freeaddrinfo(addr_info);

  // Listen on a socket
  if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
    printf("Listen failed with error: %d\n", WSAGetLastError());
    closesocket(server_socket);
    return INVALID_SOCKET;
  }

  return server_socket;
}

/**
 * @brief Close socket if valid and clean with WSACleanup.
 *
 * @param server_socket `SOCKET` for the listening server socket.
 */
void server_teardown(SOCKET server_socket) {
  if (server_socket != INVALID_SOCKET) {
    closesocket(server_socket);
  }
  WSACleanup();
}

/**
 * @brief Handle connection established by clients.
 *
 * @param server_socket `SOCKET` for the listening server socket.
 */
int handle_connection(SOCKET server_socket) {
  SOCKET client_socket = INVALID_SOCKET;
  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    return 1;
  }

  if (handle_client(client_socket)) {
    closesocket(client_socket);
    return 1;
  }

  // Shutdown the send half of the connection since no more data will be sent
  int wsaResult = shutdown(client_socket, SD_SEND);
  if (wsaResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(client_socket);
    return 1;
  }

  closesocket(client_socket);
  return 0;
}
