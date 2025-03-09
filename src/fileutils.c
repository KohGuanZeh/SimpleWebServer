#include "fileutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "strutils.h"

#define MAX_PATH_LEN 2048

const char *ROOT_FOLDER = "root";
const char *INDEX_FILE = "index.html";

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
 * @brief Returns the full path to the index file.
 *
 * @return Returns the full path to index.html.
 */
char *index_filepath() {
  static char *index_filepath[MAX_PATH_LEN + 1] = {'\0'};
  if (index_filepath[0] == NULL) {
    char *root_directory = get_root_directory();
    size_t root_dir_len = strlen(root_directory);
    strncpy(index_filepath, root_directory, root_dir_len);
    strcat(index_filepath, INDEX_FILE);
  }
  return index_filepath;
}

/**
 * @brief Normalizes the filepath to be an absolute path.
 * Function does not perform any memory allocation.
 *
 * @param rel_path `char *` relative path.
 * @return Returns the normalized absolute path on success, NULL on error.
 */
char *resolve_filepath(char *rel_path) {
  static char *root_directory;
  static size_t root_dir_len = 0;
  if (root_dir_len == 0) {
    root_directory = get_root_directory();
    size_t root_dir_len = strlen(root_directory);
  }
  size_t rel_path_len = strlen(rel_path);
  char full_path[root_dir_len + rel_path_len + 1];
  strncpy(full_path, root_directory, root_dir_len);
  strcat(full_path, rel_path);
  full_path[root_dir_len + rel_path_len] = '\0';
  if (!_fullpath(full_path, full_path, strlen(full_path))) {
    return NULL;
  }
  return full_path;
}

/**
 * @brief Returns the mime type of the file.
 * If the file is without extension, it defaults to text/plain.
 *
 * @param filepath filepath with extension.
 * @return Returns the mime type of the file.
 */
char *get_mime_type(char *filepath) {
  char *last_backslash = strchr(filepath, '\\');
  char *extension = strrchr(filepath, '.');
  if (!extension || last_backslash > extension) {
    // If extension is NULL or backslash is after extension.
    printf("No extension found");
    return DEFAULT_MIME_TYPE;
  }
  if (strcmp(extension, ".html") == 0) {
    return "text/html";
  }
  if (strcmp(extension, ".css") == 0) {
    return "text/css";
  }
  if (strcmp(extension, ".js") == 0) {
    return "application/javascript";
  }
  if (strcmp(extension, ".json") == 0) {
    return "application/json";
  }
  if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
    return "image/jpeg";
  }
  if (strcmp(extension, ".png") == 0) {
    return "image/png";
  }
  if (strcmp(extension, ".gif") == 0) {
    return "image/gif";
  }
  if (strcmp(extension, ".pdf") == 0) {
    return "application/pdf";
  }
  return DEFAULT_MIME_TYPE;
}

/**
 * @brief Add response body based on given request path.
 *
 * @param req_path Path of file that is requested.
 * @param response `Response *` that stores response information.
 * @return Returns 0 on success, 1 on error.
 */
unsigned char get_response_body_from_file(char *req_path, Response *response) {
  if (req_path == NULL || response == NULL) {
    return 1;
  }

  char *filepath = req_path;
  if (strcmp(req_path, get_root_directory()) == 0) {
    filepath = index_filepath();
  }

  FILE *fp = fopen(filepath, "r");
  if (fp == NULL) {
    return 1;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    fclose(fp);
    return 1;
  }
  long buff_size = ftell(fp);
  if (buff_size == -1) {
    fclose(fp);
    return 1;
  }
  rewind(fp);

  response->body = malloc(sizeof(char) * buff_size);
  if (response->body == NULL) {
    fclose(fp);
    return 1;
  }
  response->malloc_body = 1;

  size_t len = fread(response->body, sizeof(char), buff_size, fp);
  if (ferror(fp)) {
    printf("Error reading file: %s", filepath);
    fclose(fp);
    return 1;
  }
  fclose(fp);

  response->content_length = len;
  response->content_type = get_mime_type(filepath);
  return 0;
}