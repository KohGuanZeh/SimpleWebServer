#ifndef STRUTILS
#define STRUTILS

char* split_string(char*, char*, char*);
void replace_backslash(char*);
int hex_to_int(char);
boolean url_decode(char*);

#endif