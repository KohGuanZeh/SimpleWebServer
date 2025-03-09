#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <stdlib.h>

#include "http_types.h"

#define CRLF "\r\n"

Request *create_request();
void cleanup_request(Request *);
int parse_request_line(Request *, char *);
int parse_request_header(Request *, char *);

Response *handle_request(Request *);
void cleanup_response(Response *);
char *build_response_buffer(Response *, size_t *);
void cleanup_response_buffer(char *);

#endif