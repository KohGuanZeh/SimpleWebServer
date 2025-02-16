#include "client_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "strutils.h"

#define CRLF "\r\n"
#define REQ_LINE_DELIMITER " "
#define HEADER_DELIMITER ": "

typedef struct {
  char *http_method;
  char *path;
  char *http_version;
  size_t content_length;
  char *body;
} Request;

typedef struct {
  size_t content_length;
  char *body;
} ResponseBody;

Request *create_request();
void cleanup_request(Request *);
int parse_request_line(Request *, char *);
int parse_request_header(Request *, char *);

int handle_response(SOCKET);
ResponseBody *read_file(char *);
void cleanup_response(ResponseBody *);

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

  memset(buffer, 0, MAX_BUFFER_SIZE + 1);
  while (1) {
    if (buffer_size > MAX_BUFFER_SIZE) {
      printf("Error. Buffer size exceeds maximum limit.\n");
    }
    unsigned int recv_size = MAX_BUFFER_SIZE - buffer_size;
    int recv_result = recv(client_socket, buffer + buffer_size, recv_size, 0);
    // No more data received.
    if (recv_result == 0) {
      printf("Client connection closing...\n");
      break;
    } else if (recv_result < 0) {
      printf("recv failed: %d\n", WSAGetLastError());
      return 1;
    }
    // Set null character.
    unsigned int total_size = buffer_size + recv_result + 1;
    buffer[total_size - 1] = 0;

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
      printf("Failed to allocate memory for request line components...\n");
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
        // Receive more from buffer unless recv is 0.
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

    if (handle_response(client_socket)) {
      printf("Failed to respond.\n");
      return 1;
    }
  }

  return 0;
}

/**
 * @brief Creates an empty `Request` struct.
 *
 * @return Returns an empty `Request *` on success, `NULL` on failure.
 */
Request *create_request() {
  Request *request = malloc(sizeof(Request));
  if (request == NULL) {
    printf("Failed to create request...\n");
    return NULL;
  }
  request->http_method = NULL;
  request->path = NULL;
  request->http_version = NULL;
  request->content_length = 0;
  request->body = NULL;
  return request;
}

/**
 * @brief Cleanup `Request` struct.
 *
 * @param request `Request` struct to be cleaned.
 */
void cleanup_request(Request *request) {
  if (request == NULL) {
    return;
  }
  free(request->http_method);
  free(request->http_version);
  free(request->path);
  free(request->body);
  free(request);
}

/**
 * @brief Parses request line to `Request` struct.
 *
 * @param request `Request *` struct to store request line information.
 * @param request_line `char *` request line buffer.
 * @return Returns 0 on success, 1 on failure.
 */
int parse_request_line(Request *request, char *request_line) {
  char split = 0;
  char *next_param = split_string(request_line, REQ_LINE_DELIMITER, &split);
  if (!split) {
    printf("Failed to split for request line...\n");
    return 1;
  }
  request->http_method = malloc((strlen(request_line) + 1) * sizeof(char));
  strcpy(request->http_method, request_line);
  request_line = next_param;

  next_param = split_string(request_line, " ", &split);
  if (!split) {
    printf("Failed to split for request line...\n");
    return 1;
  }
  request->path = malloc((strlen(request_line) + 1) * sizeof(char));
  strcpy(request->path, request_line);
  request->http_version = malloc((strlen(next_param) + 1) * sizeof(char));
  strcpy(request->http_version, next_param);

  if (request->http_method == NULL || request->http_version == NULL ||
      request->path == NULL) {
    return 1;
  }
  return 0;
}

/**
 * @brief Parses reuqest header to `Request` struct.
 * Only content-length is read as of now to handle request body if any.
 *
 * @param request `Request *` struct to store request header information.
 * @param header `char *` request header buffer.
 * @return Returns 0 on success, 1 on failure.
 */
int parse_request_header(Request *request, char *header) {
  char split = 0;
  char *value = split_string(header, HEADER_DELIMITER, &split);
  if (!split) {
    printf("Failed to split for request header...\n");
    return 1;
  }
  header = strlwr(header);
  if (strcmp(header, "content-length") != 0) {
    return 0;
  }

  char *end_ptr;
  request->content_length = strtoull(value, &end_ptr, 10);
  if (end_ptr != 0) {
    return 1;
  }
  return 0;
}

/**
 * @brief Handle creation and sending of response.
 *
 * @param client_socket `SOCKET *` for the client socket.
 * @return Returns 0 on success, 1 on failure.
 */
int handle_response(SOCKET client_socket) {
  char *response_buffer = "HTTP/1.0 200 OK\r\nContent-Length: 5\r\n\r\nHello";
  // int response_size = 0;

  int send_result =
      send(client_socket, response_buffer, strlen(response_buffer) + 1, 0);
  if (send_result == SOCKET_ERROR) {
    printf("send failed: %d\n", WSAGetLastError());
    return 1;
  }
  printf("Bytes sent: %d\n\n", send_result);
  return 0;
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

  res_body->body = malloc(sizeof(char) * buff_size + 1);

  if (res_body->body == NULL) {
    fclose(fp);
    return NULL;
  }

  if (fseek(fp, 0, SEEK_SET) != 0) {
    // Error encountered on fseek
    fclose(fp);
    return NULL;
  }

  size_t len = fread(res_body->body, sizeof(char), buff_size, fp);
  if (ferror(fp)) {
    printf("Error reading file: %s", file_name);
    fclose(fp);
    return NULL;
  }

  res_body->body[len++] = 0;
  res_body->content_length = len;
  return res_body;
}

void cleanup_response(ResponseBody *body) {
  free(body->body);
  free(body);
}