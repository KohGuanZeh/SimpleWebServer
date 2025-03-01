#ifndef STRUTILS
#define STRUTILS

char *split_string(char *, char *, char *);
void replace_backslash(char *);
int hex_to_int(char);
unsigned char url_decode(char *);
void strcpy_newbuf(char *, char *);

#endif