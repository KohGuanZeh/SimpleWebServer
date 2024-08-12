#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define HTTP_PORT "80"

WSADATA wsaData;

int main() {
  int wsaResult;

  // Initialize Winsock
  wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (wsaResult != 0) {
    printf("WSAStartup failed: %d\n", wsaResult);
    return 1;
  }

  // Create socket for server
  struct addrinfo *result = NULL, *ptr = NULL, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  wsaResult = getaddrinfo(NULL, HTTP_PORT, &hints, &result);
  if (wsaResult != 0) {
    printf("getaddrinfo failed: %d\n", wsaResult);
    WSACleanup();
    return 1;
  }

  // Create socket for server to listen for client connections
  SOCKET listenSocket = INVALID_SOCKET;
  listenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (listenSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  // Setup the TCP listening socket
  wsaResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (wsaResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }
  // Free memory allocated by getaddrinfo()
  freeaddrinfo(result);

  // Listen on a socket
  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    printf("Listen failed with error: %ld\n", WSAGetLastError());
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  // Accept client socket
  SOCKET clientSocket = INVALID_SOCKET;
  clientSocket = accept(listenSocket, NULL, NULL);
  if (clientSocket == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  // Close socket after implementation
  closesocket(listenSocket);

  // Shutdown the send half of the connection since no more data will be sent
  wsaResult = shutdown(clientSocket, SD_SEND);
  if (wsaResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(clientSocket);
    WSACleanup();
    return 1;
  }
  closesocket(clientSocket);
  WSACleanup();

  return 0;
}
