#ifndef HTTP_TYPES
#define HTTP_TYPES

#include <stdlib.h>

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
  char *body;
} Response;

#endif