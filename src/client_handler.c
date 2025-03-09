#include "client_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_parser.h"
#include "strutils.h"

int fill_buffer(SOCKET, char *, unsigned int *);

/**
 * @brief Handles client socket.
 *
 * @param client_socket `SOCKET` for the client socket.
 * @return Returns 0 on success, 1 on failure.
 */
int handle_client(SOCKET client_socket) {
  char buffer[MAX_BUFFER_SIZE + 1];
  unsigned int buffer_size = 0;
  int recv_result = 0;
  char split = 0;

  while (1) {
    recv_result = fill_buffer(client_socket, buffer, &buffer_size);
    if (recv_result == 0) {
      printf("Client connection closing...\n");
      break;
    } else if (recv_result < 0) {
      printf("recv failed: %d\n", WSAGetLastError());
      return 1;
    } else if (recv_result == MAX_BUFFER_SIZE + 1) {
      printf("Error. Buffer size exceeds maximum limit.\n");
      return 1;
    }

    // Request Builder.
    Request *request = create_request();
    if (request == NULL) {
      return 1;
    }

    char *temp_buffer = buffer;
    char *next_buffer = split_string(temp_buffer, CRLF, &split);
    if (!split) {
      printf("Buffer is too small to receive request...\n");
      cleanup_request(request);
      return 1;
    }
    if (parse_request_line(request, temp_buffer)) {
      printf("Failed to parse request line...\n");
      cleanup_request(request);
      return 1;
    }

    printf("HTTP Method: %s\n", request->http_method);
    printf("HTTP Version: %s\n", request->http_version);
    printf("Path: %s\n", request->path);

    // Loop until a full request has been built and handled.
    while (1) {
      temp_buffer = next_buffer;
      next_buffer = split_string(temp_buffer, CRLF, &split);
      if (!split) {
        buffer_size = strlen(temp_buffer);
        // If buffer is already full:
        if (strlen(temp_buffer) >= MAX_BUFFER_SIZE) {
          printf("Buffer is too small to receive request...\n");
          cleanup_request(request);
          return 1;
        }
        memcpy(buffer, temp_buffer, buffer_size);
        recv_result = fill_buffer(client_socket, buffer, &buffer_size);
        if (recv_result >= 0 && recv_result <= MAX_BUFFER_SIZE) {
          next_buffer = buffer;
          continue;
        }
        if (recv_result < 0) {
          printf("recv failed: %d\n", WSAGetLastError());
        } else if (recv_result == MAX_BUFFER_SIZE + 1) {
          printf("Error. Buffer size exceeds maximum limit.\n");
        }
        cleanup_request(request);
        return 1;
      }
      if (strcmp(temp_buffer, "") == 0) {
        // End of request headers.
        break;
      }
      if (parse_request_header(request, temp_buffer)) {
        printf("Failed to process request headers...\n");
        cleanup_request(request);
        return 1;
      }
    }

    printf("Content-Length: %Iu\n\n", request->content_length);

    Response *response = handle_request(request);
    cleanup_request(request);
    if (response == NULL) {
      printf("Failed to allocate memory. Aborting...\n");
      return 1;
    }

    size_t send_size = 0;
    unsigned char clean_buffer = 1;
    char *send_buffer = build_response_buffer(response, &send_size);
    cleanup_response(response);

    if (send_buffer == NULL) {
      continue;
    }

    int send_result = send(client_socket, send_buffer, send_size, 0);
    cleanup_response_buffer(send_buffer);
    if (send_result == SOCKET_ERROR) {
      printf("send failed: %d\n", WSAGetLastError());
      return 1;
    }
    printf("Bytes sent: %d\n\n", send_result);
  }

  return 0;
}

/**
 * @brief Fills receive buffer with socket data and return receive result.
 * Buffer size is also updated within this function.
 *
 * @param client_socket `SOCKET` of client.
 * @param buffer `char *` buffer to receive data.
 * @param buffer_size_ptr `unsigned int *` of current size of buffer.
 * @return Returns socket receive result.
 */
int fill_buffer(SOCKET client_socket, char *buffer,
                unsigned int *buffer_size_ptr) {
  unsigned int buffer_size = *buffer_size_ptr;
  if (buffer_size >= MAX_BUFFER_SIZE) {
    return MAX_BUFFER_SIZE + 1;
  }
  char *buffer_start = buffer + buffer_size;
  memset(buffer_start, '\0', MAX_BUFFER_SIZE - buffer_size);
  unsigned int recv_size = MAX_BUFFER_SIZE - buffer_size;
  int recv_result = recv(client_socket, buffer + buffer_size, recv_size, 0);
  if (recv_result > 0) {
    *buffer_size_ptr = buffer_size + recv_result;
  }
  return recv_result;
}