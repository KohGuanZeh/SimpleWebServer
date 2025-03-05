#ifndef FILEUTILS
#define FILEUTILS

#define DEFAULT_MIME_TYPE "text/txt"

char *get_root_directory();
char *resolve_filepath(char *);
char *get_mime_type(char *);

#endif