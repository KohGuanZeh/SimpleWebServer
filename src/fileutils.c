#include "fileutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "strutils.h"

#define MAX_PATH_LEN 2048

const char *ROOT_FOLDER = "root";

/**
 * @brief Returns the root directory of where webserver files are served.
 * Function does not perform any memory allocation.
 *
 * @return Directory of the webserver files to be served.
 */
char *get_root_directory() {
  static char root_directory[MAX_PATH_LEN + 1] = {'\0'};
  if (root_directory[0] != '\0') {
    // Root directory is cached.
    return root_directory;
  }
  GetModuleFileName(NULL, root_directory, MAX_PATH_LEN);
  char *last_fslash = strrchr(root_directory, '\\');
  if (last_fslash) {
    char *next = last_fslash + 1;
    while (*next != '\0') {
      *next = '\0';
      next++;
    }
  }
  strcat(root_directory, ROOT_FOLDER);
  return root_directory;
}

/**
 * @brief Normalizes the filepath to be an absolute path.
 * Function does not perform any memory allocation.
 *
 * @param rel_path `char *` relative path.
 * @return Returns the normalized absolute path on success, NULL on error.
 */
char *resolve_filepath(char *rel_path) {
  char *root_directory = get_root_directory();
  size_t root_dir_len = strlen(root_directory);
  size_t rel_path_len = strlen(rel_path);
  char full_path[root_dir_len + rel_path_len + 1];
  strncpy(full_path, root_directory, root_dir_len);
  strncpy(full_path + root_dir_len, rel_path, rel_path_len);
  full_path[root_dir_len + rel_path_len] = '\0';
  if (!_fullpath(full_path, full_path, strlen(full_path))) {
    return NULL;
  }
  return full_path;
}

/**
 * @brief Returns the mime type of the file.
 * If the file is without extension, it defaults to text/txt.
 *
 * @param file `char *` that is the path of the file with extension.
 * @return Returns the mime type of the file.
 */
char *get_mime_type(char *file) { return DEFAULT_MIME_TYPE; }