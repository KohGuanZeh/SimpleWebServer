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
    last_fslash[1] = '\0';
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
  static char index_filepath[MAX_PATH_LEN + 1] = {'\0'};
  if (index_filepath[0] == '\0') {
    int written = snprintf(index_filepath, MAX_PATH_LEN + 1, "%s\\%s",
                           get_root_directory(), INDEX_FILE);
    if (written < 0 || written >= MAX_PATH_LEN) {
      printf("Error: index filepath exceeds MAX_PATH_LEN.\n");
      return NULL;
    }
  }
  return index_filepath;
}

/**
 * @brief Normalizes the filepath to be an absolute path.
 * Note that this function performs memory allocation.
 *
 * @param rel_path `char *` relative path.
 * @return Returns the normalized absolute path on success, NULL on error.
 */
char *resolve_filepath(char *rel_path) {
  char abs_path[MAX_PATH_LEN + 1] = {'\0'};
  int written = snprintf(abs_path, MAX_PATH_LEN + 1, "%s%s",
                         get_root_directory(), rel_path);
  if (written < 0 || written >= MAX_PATH_LEN) {
    printf("Error: full filepath exceeds MAX_PATH_LEN.\n");
    return NULL;
  }
  char *full_path = calloc(MAX_PATH_LEN + 1, sizeof(char));
  if (full_path == NULL) {
    return NULL;
  }
  if (_fullpath(full_path, abs_path, MAX_PATH_LEN) == NULL) {
    free(full_path);
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
  static size_t root_directory_len = 0;
  if (root_directory_len == 0) {
    root_directory_len = strlen(get_root_directory());
  }
  if (req_path == NULL || response == NULL) {
    return 1;
  }

  char *filepath = req_path;
  if (strcmp(req_path + root_directory_len, "\\") == 0) {
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

  size_t len = fread(response->body, sizeof(char), buff_size, fp);
  if (ferror(fp)) {
    printf("Error reading file: %s", filepath);
    fclose(fp);
    return 1;
  }
  fclose(fp);

  response->content_length = len;
  response->content_type = strdup(get_mime_type(filepath));
  if (response->content_type == NULL) {
    return 1;
  }
  return 0;
}