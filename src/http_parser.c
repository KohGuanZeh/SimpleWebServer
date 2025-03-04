#include "http_parser.h"

#include <stdio.h>
#include <string.h>

#include "fileutils.h"
#include "strutils.h"

#define RESPONSE_TEMPLATE_LEN 4096
#define CONTENT_LENGTH_LEN 20
#define REQ_LINE_DELIMITER " "
#define HEADER_DELIMITER ": "

#define HTTP_VERSION "HTTP/1.0"
#define STATUS_500 "500 Internal Server Error"
#define STATUS_400 "400 Bad Request"
#define STATUS_403 "403 Forbidden"

Response create_empty_response();
Response create_status_400_response(char *);
Response create_status_500_response(char *);

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
 * @return Returns an empty `Response`.
 */
Response create_empty_response() {
  Response response = {.status = NULL,
                       .content_length = 0,
                       .content_type = NULL,
                       .body = NULL,
                       .malloc_body = 0};
  return response;
}

/**
 * @brief Creates a status 400 `Response` struct.
 *
 * @param msg `char *` String based error message to return as response.
 * `NULL` if no error message is to be transmitted.
 * @return Returns an empty `Response`.
 */
Response create_status_400_response(char *msg) {
  Response response = {.status = NULL,
                       .content_length = msg != NULL ? strlen(msg) : 0,
                       .content_type = NULL,
                       .body = msg,
                       .malloc_body = 0};
  return response;
}

/**
 * @brief Creates a status 500 `Response` struct.
 *
 * @param msg `char *` String based error message to return as response.
 * `NULL` if no error message is to be transmitted.
 * @return Returns a status 500 `Response` with error message.
 */
Response create_status_500_response(char *msg) {
  Response response = {.status = NULL,
                       .content_length = msg != NULL ? strlen(msg) : 0,
                       .content_type = NULL,
                       .body = msg,
                       .malloc_body = 0};
  return response;
}

/**
 * @brief Builds a response object given the request.
 *
 * @param request `Request *` struct that stores request information.
 * @return Returns a Response object for the request.
 */
Response handle_request(Request *request) {
  if (url_decode(request->path)) {
    // If bad string, send bad request error.
    return create_status_400_response("URL decode error. Bad request.");
  }
  char *filepath = resolve_filepath(request->path);
  if (filepath == NULL) {
    return create_status_500_response("Unable to resolve filepath.");
  }
  Response response = create_empty_response();
  return response;
}

/**
 * @brief Cleanup the `Response` struct.
 * Frees dynamically allocated memory for the response body.
 *
 * @param response `Response *` struct to be cleaned.
 */
void cleanup_response(Response *response) {
  if (response == NULL) {
    return;
  }
  if (response->malloc_body) {
    // Free body if it is dynamically allocated.
    free(response->body);
  }
}

char *build_response_buffer(Response *response, size_t *size) {
  Response response_value;
  if (response == NULL) {
    response_value = create_status_500_response(NULL);
  }
  response_value = *response;
}

/**
 * @brief Cleanup the response buffer.
 *
 * @param response_buffer `char *` buffer to be cleaned.
 */
void cleanup_response_buffer(char *response_buffer) { free(response_buffer); }