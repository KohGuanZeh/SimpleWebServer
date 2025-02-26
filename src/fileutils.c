#include "fileutils.h"

#include <stdio.h>
#include <windows.h>

#include "strutils.h"

#define MAX_PATH_LEN 8192

const char *ROOT_FOLDER = "/root";

char *get_exe_directory();

/**
 * @brief Returns the directory of the executable.
 * Note that memory is allocated on the heap and needs to be freed.
 *
 * @return Directory of executable location.
 */
char *get_exe_directory() {
  char exe_path[MAX_PATH_LEN + 1] = {'\0'};
  GetModuleFileName(NULL, exe_path, MAX_PATH_LEN);
  replace_backslash(exe_path);

  char *last = strrchr(exe_path, '/');
  if (last) {
    *last = '\0';
  }

  size_t dir_len = strlen(exe_path);
  char *dir = calloc(dir_len + 1, sizeof(char));
  if (dir == NULL) {
    printf("Failed to malloc for directory...");
    return NULL;
  }
  strncpy(dir, exe_path, dir_len);
  dir[dir_len] = '\0';
  return dir;
}

/**
 * @brief Returns the root directory of where webserver files are served.
 * Note that memory is allocated on the heap and needs to be freed.
 *
 * @return Directory of the webserver files to be served.
 */
char *get_root_directory() {
  char *dir = get_exe_directory();
  if (dir == NULL) {
    return NULL;
  }
  size_t new_dir_len = strlen(dir) + strlen(ROOT_FOLDER);
  char *root_dir = realloc(dir, new_dir_len + 1);
  if (root_dir != dir) {
    free(dir);
  }
  if (root_dir == NULL) {
    printf("Failed to realloc for root directory...");
    return NULL;
  }
  strcat(root_dir, ROOT_FOLDER);
  root_dir[new_dir_len] = '\0';
  return root_dir;
}