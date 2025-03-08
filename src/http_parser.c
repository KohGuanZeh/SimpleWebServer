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
Response create_error_response(char *, char *, unsigned char);

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
 * @brief Creates an error-based `Response` struct.
 *
 * @param status Status line of response.
 * @param msg String error message to return as response.
 * `NULL` if no error message is to be transmitted.
 * @param malloc_body `1` if the `msg` is dynamically allocated. `0` otherwise.
 * @return Returns an error-based `Response`.
 */
Response create_error_response(char *status, char *msg,
                               unsigned char malloc_body) {
  Response response = {.status = status,
                       .content_length = msg == NULL ? 0 : strlen(msg),
                       .content_type = DEFAULT_MIME_TYPE,
                       .body = msg,
                       .malloc_body = malloc_body};
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
    // If bad string, return bad request.
    return create_error_response(STATUS_400, "Bad Request: URL decode error.",
                                 0);
  }
  char *filepath = resolve_filepath(request->path);
  if (filepath == NULL) {
    // If unable to resolve filepath, return internal server error.
    return create_error_response(
        STATUS_500, "Internal Server Error: Unable to resolve filepath.", 0);
  }

  static char *root_directory;
  static size_t root_dir_len = 0;
  if (root_dir_len == 0) {
    root_directory = get_root_directory();
    root_dir_len = strlen(root_directory);
  }
  if (strncmp(filepath, root_directory, root_dir_len) != 0) {
    // If resolved file path is outside of root directory, return forbidden.
    return create_error_response(STATUS_403, "Forbidden Access.", 0);
  }

  Response response = create_empty_response();
  if (generate_response_body(filepath, &response)) {
    cleanup_response(&response);
    return create_error_response(STATUS_500,
                                 "Failed to generate response body.", 0);
  }
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
    response_value =
        create_error_response(STATUS_500, "Internal Server Error.", 0);
  }
  response_value = *response;
}

/**
 * @brief Cleanup the response buffer.
 *
 * @param response_buffer `char *` buffer to be cleaned.
 */
void cleanup_response_buffer(char *response_buffer) { free(response_buffer); }