#include <stdlib.h>

#define STATUS_500_INTERNAL_SERVER_ERROR "500 Internal Server Error"

typedef struct {
  char *http_method;
  char *path;
  char *http_version;
  size_t content_length;
  char *body;
} Request;

typedef struct {
  char *http_version;
  char *status;
  size_t content_length;
  char *content_type;
  char *body;
} Response;

Request *create_request();
void cleanup_request(Request *);
int parse_request_line(Request *, char *);
int parse_request_header(Request *, char *);

Response *handle_request(Request *);
void cleanup_response(Response *);
char *build_response_buffer(Response *, size_t *);
void cleanup_response_buffer(char *);

char *internal_server_error(size_t *);