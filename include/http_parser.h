#include <stdlib.h>

#define CRLF "\r\n"

typedef struct {
  char *http_method;
  char *path;
  char *http_version;
  size_t content_length;
  char *body;
} Request;

typedef struct {
  char *status;
  size_t content_length;
  char *content_type;
  unsigned char malloc_body;
  char *body;
} Response;

Request *create_request();
void cleanup_request(Request *);
int parse_request_line(Request *, char *);
int parse_request_header(Request *, char *);

Response handle_request(Request *);
void cleanup_response(Response *);
char *build_response_buffer(Response *, size_t *);
void cleanup_response_buffer(char *);