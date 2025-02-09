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
 * @return Returns  `char *` to the next half of the split string if it exists.
 * If not, it will return `NULL`.
 */
char *split_string(char *string, char *delimiter) {
  if (string == NULL) {
    return NULL;
  }
  size_t delim_len = strlen(delimiter);
  char *delim_ptr = strstr(string, delimiter);
  if (delim_ptr == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < delim_len; i++) {
    *delim_ptr = '\0';
    delim_ptr += 1;
  }
  return delim_ptr;
}
