#include <string.h>

/**
 * @brief Splits the string based on first instance of delimiter found.
 * The delimiter characters in the string will be replaced will `NULL`
 * characters to split the string. `char *string` will then remain to point to
 * the first part of the string while the `char *` returned will be the second
 * half of the string.
 *
 * @param string String to be split based on the delimiter.
 * @param delimiter String to function as a delimiter.
 * @param split `char *` that stores 1 if it did split, 0 otherwise.
 * @return Returns  `char *` to the next half of the split string if it exists.
 * If not, it will return `NULL`.
 */
char *split_string(char *string, char *delimiter, char *split) {
  *split = 0;
  if (string == NULL) {
    return NULL;
  }
  size_t delim_len = strlen(delimiter);
  char *delim_ptr = strstr(string, delimiter);
  if (delim_ptr == NULL) {
    return NULL;
  }
  *split = 1;
  for (size_t i = 0; i < delim_len; i++) {
    *delim_ptr = '\0';
    delim_ptr += 1;
  }
  return delim_ptr;
}

/**
 * @brief Replaces backslash in strings with forward slash.
 */
void replace_backslash(char *string) {
  for (int i = 0; string[i] != '\0'; i++) {
    if (string[i] == '\\') {
      string[i] = '/';
    }
  }
}