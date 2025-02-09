#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "strutils.h"

// Set 8KB Buffer Length
#define BUFFER_LEN 8192

typedef struct {
  char *http_method;
  char *path;
  char *http_version;
} Request;

typedef struct {
  char *content;
  size_t content_length;
} ResponseBody;

int init_winsock();
SOCKET init_server_socket();
int handle_connection(SOCKET *);
int handle_request(SOCKET *);
Request *parse_request(char *, int);
int handle_response(SOCKET *);
ResponseBody *read_file(char *);
void cleanup_request(Request *);

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
    closesocket(client_socket);
    return 1;
  }

  closesocket(client_socket);
  return 0;
}

/**
 * @brief Handle request of client socket.
 *
 * @param client_socket `SOCKET *` for the client socket.
 * @return Returns 0 on success, 1 on failure.
 */
int handle_request(SOCKET *client_socket) {
  char recv_buffer[BUFFER_LEN + 1];
  int recv_result;

  // Receive until the peer shuts down the connection
  do {
    recv_result = recv(*client_socket, recv_buffer, BUFFER_LEN, 0);
    if (recv_result > 0) {
      printf("Bytes received: %d\n", recv_result);

      // Set to null character.
      recv_buffer[BUFFER_LEN] = 0;
      printf("Received Request: %s", recv_buffer);

      Request *req = parse_request(recv_buffer, recv_result);
      if (req == NULL) {
        printf("Failed to parse request");
        return 1;
      }

      printf("%s\n", req->http_method);
      printf("%s\n", req->http_version);
      printf("%s\n\n", req->path);

      cleanup_request(req);

      if (handle_response(client_socket)) {
        printf("Failed to respond.\n");
        return 1;
      }

    } else if (recv_result == 0) {
      printf("Client connection closing...\n");
    } else {
      printf("recv failed: %d\n", WSAGetLastError());
      return 1;
    }

  } while (recv_result >= BUFFER_LEN);

  return 0;
}

/**
 * @brief Handle parsing of request.
 *
 * @param recv_buffer `char *` that is received by the socket.
 * @return Returns `Request *` containing information of the request.
 */
Request *parse_request(char *recv_buffer, int buffer_length) {
  if (recv_buffer == NULL) {
    return NULL;
  }

  Request *req = malloc(sizeof(Request));
  if (req == NULL) {
    return NULL;
  }
  req->http_method = NULL;
  req->http_version = NULL;
  req->path = NULL;

  char *next_buffer = split_string(recv_buffer, "\n");
  char *req_buffer = recv_buffer;
  recv_buffer = next_buffer;

  char *next_req_param = split_string(req_buffer, " ");
  req->http_method = malloc(strlen(req_buffer) + 1);
  strcpy(req->http_method, req_buffer);
  req_buffer = next_req_param;

  next_req_param = split_string(req_buffer, " ");
  req->path = malloc(strlen(req_buffer) + 1);
  strcpy(req->path, req_buffer);
  req->http_version = malloc(strlen(next_req_param) + 1);
  strcpy(req->http_version, next_req_param);

  if (req->http_method == NULL || req->http_version == NULL ||
      req->path == NULL) {
    cleanup_request(req);
    return NULL;
  }
  return req;
}

/**
 * @brief Handle creation and sending of response.
 *
 * @param client_socket `SOCKET *` for the client socket.
 * @return Returns 0 on success, 1 on failure.
 */
int handle_response(SOCKET *client_socket) {
  char *response_buffer = "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nHello";
  // int response_size = 0;

  int send_result =
      send(*client_socket, response_buffer, strlen(response_buffer) + 1, 0);
  if (send_result == SOCKET_ERROR) {
    printf("send failed: %d\n", WSAGetLastError());
    return 1;
  }
  printf("Bytes sent: %d\n", send_result);
  return 0;
}

void cleanup_request(Request *req) {
  free(req->http_method);
  free(req->http_version);
  free(req->path);
  free(req);
}

ResponseBody *read_file(char *file_name) {
  ResponseBody *res_body = malloc(sizeof(ResponseBody));
  if (res_body == NULL) {
    return NULL;
  }

  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    return NULL;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    // Error encountered on fseek
    fclose(fp);
    return NULL;
  }
  long buff_size = ftell(fp);
  if (buff_size == -1) {
    fclose(fp);
    return NULL;
  }

  res_body->content = malloc(sizeof(char) * buff_size + 1);
  res_body->content_length = malloc(sizeof(size_t));

  if (res_body->content == NULL || res_body->content_length == NULL) {
    fclose(fp);
    return NULL;
  }

  if (fseek(fp, 0, SEEK_SET) != 0) {
    // Error encountered on fseek
    fclose(fp);
    return NULL;
  }

  size_t len = fread(res_body->content, sizeof(char), buff_size, fp);
  if (ferror(fp)) {
    printf("Error reading file: %s", file_name);
    fclose(fp);
    return NULL;
  }

  res_body->content[len++] = 0;
  res_body->content_length = len;
  return res_body;
}