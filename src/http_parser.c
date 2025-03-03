#include "http_parser.h"

#include <stdio.h>
#include <string.h>

#include "fileutils.h"
#include "strutils.h"

#define REQ_LINE_DELIMITER " "
#define HEADER_DELIMITER ": "

#define HTTP_VERSION "HTTP/1.0"
#define STATUS_400 "400 Bad Request"
#define STATUS_403 "403 Forbidden"

Response *create_empty_response();

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
  strcpy_newbuf(request->http_method, request_line);
  if (request->http_method == NULL) {
    return 1;
  }
  request_line = next_param;

  next_param = split_string(request_line, " ", &split);
  if (!split) {
    printf("Failed to split for request line...\n");
    return 1;
  }
  strcpy_newbuf(request->path, request_line);
  strcpy_newbuf(request->http_version, next_param);
  if (request->http_version == NULL || request->path == NULL) {
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
  printf("%s\n", header);
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
 * @brief Creates an empty `Response` struct.
 *
 * @return Returns an empty `Response *` on success, `NULL` on failure.
 */
Response *create_empty_response() {
  Response *response = malloc(sizeof(Response));
  if (response == NULL) {
    return NULL;
  }
  response->http_version = NULL;
  response->status = NULL;
  response->content_length = 0;
  response->content_type = NULL;
  response->body = NULL;
  return response;
}

/**
 * @brief Builds a response object given the request.
 *
 * @param request `Request *` struct that stores request information.
 * @return Returns a Response object for the request.
 */
Response *handle_request(Request *request) {
  Response *response = create_empty_response();
  strcpy_newbuf(response->http_version, HTTP_VERSION);
  if (response->http_version == NULL) {
    return NULL;
  }
  if (url_decode(request->path)) {
    // If bad string, send bad request error.
    strcpy_newbuf(response->status, STATUS_400);
    if (response->status == NULL) {
      return NULL;
    }
    return response;
  }
  char *filepath = resolve_filepath(request->path);
  if (filepath == NULL) {
  }
  return response;
}

/**
 * @brief Cleanup the `Response` struct.
 *
 * @param response `Response *` struct to be cleaned.
 */
void cleanup_response(Response *response) {
  if (response == NULL) {
    return;
  }
  free(response->http_version);
  free(response->status);
  free(response->content_type);
  free(response->body);
  free(response);
}

char *build_response_buffer(Response *response, size_t *size) {
  if (response == NULL) {
    return NULL;
  }
}

/**
 * @brief Cleanup the response buffer.
 *
 * @param response_buffer `char *` buffer to be cleaned.
 */
void cleanup_response_buffer(char *response_buffer) { free(response_buffer); }

/**
 * @brief Return buffer for internal server error.
 *
 * @param size `size_t *` to store size of return buffer.
 * @return Returns the buffer containing the server error response.
 */
char *internal_server_error(size_t *size) {
  char *buff =
      "HTTP/1.0 500 Internal Server Error\r\nContent-Type: "
      "text/html\r\nConnection: close\r\n\r\n";
  *size = strlen(buff) + 1;
  return buff;
}