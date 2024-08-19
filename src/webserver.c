#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int init_winsock();
int init_server_socket(SOCKET *);

int main() {
  if (init_winsock()) {
    return EXIT_FAILURE;
  }

  SOCKET server_socket = INVALID_SOCKET;
  if (init_server_socket(&server_socket)) {
    return EXIT_FAILURE;
  }

  // Accept client socket
  SOCKET client_socket = INVALID_SOCKET;
  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(server_socket);
    WSACleanup();
    return EXIT_FAILURE;
  }

  // Shutdown the send half of the connection since no more data will be sent
  int wsaResult = shutdown(client_socket, SD_SEND);
  if (wsaResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(client_socket);
    WSACleanup();
    return EXIT_FAILURE;
  }
  closesocket(client_socket);

  // Close socket after implementation
  closesocket(server_socket);

  WSACleanup();

  return EXIT_SUCCESS;
}

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
 * @param listen_socket SOCKET to initialize the server listening socket.
 * @return Returns 0 on success, 1 on failure.
 */
int init_server_socket(SOCKET *server_socket) {
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
    WSACleanup();
    return 1;
  }

  // Create socket for server to listen for client connections
  *server_socket = socket(addr_info->ai_family, addr_info->ai_socktype,
                          addr_info->ai_protocol);

  if (*server_socket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(addr_info);
    WSACleanup();
    return 1;
  }

  // Setup the TCP listening socket
  wsaResult =
      bind(*server_socket, addr_info->ai_addr, (int)addr_info->ai_addrlen);
  if (wsaResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(addr_info);
    closesocket(*server_socket);
    WSACleanup();
    return 1;
  }

  // Free memory allocated by getaddrinfo()
  freeaddrinfo(addr_info);

  // Listen on a socket
  if (listen(*server_socket, SOMAXCONN) == SOCKET_ERROR) {
    printf("Listen failed with error: %ld\n", WSAGetLastError());
    closesocket(*server_socket);
    WSACleanup();
    return 1;
  }

  return 0;
}