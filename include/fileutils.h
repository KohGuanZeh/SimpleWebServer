#ifndef FILEUTILS
#define FILEUTILS

#include "http_types.h"

#define DEFAULT_MIME_TYPE "text/plain"

char *get_root_directory();
char *index_filepath();
char *resolve_filepath(char *);
char *get_mime_type(char *);

unsigned char generate_response_body(char *, Response *);

#endif