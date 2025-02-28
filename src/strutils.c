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

/**
 * @brief Converts a hex character to its corresponding int value.
 * Returns -1 if it is not valid.
 *
 * @param hex_char `char` hex character to be converted to integer.
 * @return Returns value of hex character, -1 if invalid hex character.
 */
int hex_to_int(char hex_char) {
  if (hex_char >= '0' && hex_char <= '9') {
    return hex_char - '0';
  }
  if (hex_char >= 'A' && hex_char <= 'F') {
    return hex_char - 'A' + 10;
  }
  if (hex_char >= 'a' && hex_char <= 'f') {
    return hex_char - 'a' + 10;
  }
  return -1;
}

/**
 * @brief Decodes the URL string in place.
 *
 * @param string `char *` URL string to decode.
 * @return Returns 0 if the string is parsed successfully, 1 if it is a bad
 * string.
 */
unsigned char url_decode(char *string) {
  char *seek_ptr = string;

  while (*seek_ptr != '\0') {
    if (*seek_ptr == '%') {
      seek_ptr++;
      int first_hex = hex_to_int(*seek_ptr);
      seek_ptr++;
      int second_hex = hex_to_int(*seek_ptr);
      if (first_hex < 0 || second_hex < 0) {
        return 1;
      }
      *string = first_hex << 4 + second_hex;
      if (*string == '\0') {
        return 1;
      }
    } else if (*seek_ptr == '+') {
      *string = ' ';
    } else {
      *string = *seek_ptr;
    }
    seek_ptr++;
    string++;
  }
  while (string < seek_ptr) {
    string = '\0';
    string++;
  }
  return 1;
}