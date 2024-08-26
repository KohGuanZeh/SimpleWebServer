#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_LEN 512

int init_winsock();
SOCKET init_server_socket();
int handle_connection(SOCKET *);
int handle_request(SOCKET *);

int main() {
  if (init_winsock()) {
    return EXIT_FAILURE;
  }

  SOCKET server_socket = init_server_socket();
  if (server_socket == INVALID_SOCKET) {
    WSACleanup();
    return EXIT_FAILURE;
  }

  while (TRUE) {
    if (handle_connection(&server_socket)) {
      closesocket(server_socket);
      WSACleanup();
      return EXIT_FAILURE;
    }
  }

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
 * @return Returns newly created server listening socket, on success,
 * `INVALID_SOCKET` on failure.
 */
SOCKET init_server_socket() {
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
    printf("Error at socket(): %ld\n", WSAGetLastError());
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
    printf("Listen failed with error: %ld\n", WSAGetLastError());
    closesocket(server_socket);
    return INVALID_SOCKET;
  }

  return server_socket;
}

/**
 * @brief Handle connection established by clients.
 *
 * @param server_socket `SOCKET *` for the listening server socket.
 */
int handle_connection(SOCKET *server_socket) {
  SOCKET client_socket = INVALID_SOCKET;
  client_socket = accept(*server_socket, NULL, NULL);
  if (client_socket == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    return 1;
  }

  if (handle_request(&client_socket)) {
    // Client Socket cleanup done on error.
    // Each time request fails, server will stop.
    // Todo: Create an enum to check if server socket should close.
    closesocket(client_socket);
    return 1;
  }

  // Shutdown the send half of the connection since no more data will be sent
  int wsaResult = shutdown(client_socket, SD_SEND);
  if (wsaResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
  }

  closesocket(client_socket);
  return 0;
}

/**
 * @brief Handle request of client socket
 *
 * @param client_socket `SOCKET *` for the client socket.
 * @return Returns 0 on success, 1 on failure.
 */
int handle_request(SOCKET *client_socket) {
  char recv_buffer[BUFFER_LEN];
  int recv_result;
  int send_result;

  // Receive until the peer shuts down the connection
  do {
    recv_result = recv(*client_socket, recv_buffer, BUFFER_LEN, 0);
    if (recv_result > 0) {
      printf("Bytes received: %d\n", recv_result);

      // Echo the buffer back to the sender
      send_result = send(*client_socket, recv_buffer, recv_result, 0);
      if (send_result == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        return 1;
      }
      printf("Bytes sent: %d\n", send_result);
    } else if (recv_result == 0) {
      printf("Client connection closing...\n");
    } else {
      printf("recv failed: %d\n", WSAGetLastError());
      return 1;
    }

  } while (recv_result >= BUFFER_LEN);

  printf("Done with Client request.\n");

  return 0;
}