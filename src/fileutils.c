#include "fileutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "strutils.h"

#define MAX_PATH_LEN 2048
#define DEFAULT_MIME_TYPE "application/octet-stream";

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
 * If the file is without extension, it defaults to application/octet-stream.
 * Reference:
 * https://developer.mozilla.org/en-US/docs/Web/HTTP/MIME_types/Common_types
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
  if (strcmp(extension, ".txt") == 0) {
    return "text/plain";
  }
  if (strcmp(extension, ".html") == 0) {
    return "text/html";
  }
  if (strcmp(extension, ".htm") == 0) {
    return "text/html";
  }
  if (strcmp(extension, ".css") == 0) {
    return "text/css";
  }
  if (strcmp(extension, ".js") == 0) {
    return "text/javascript";
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
  if (strcmp(extension, ".aac") == 0) {
    return "audio/aac";
  }
  if (strcmp(extension, ".abw") == 0) {
    return "application/x-abiword";
  }
  if (strcmp(extension, ".apng") == 0) {
    return "image/apng";
  }
  if (strcmp(extension, ".arc") == 0) {
    return "application/x-free-arc";
  }
  if (strcmp(extension, ".avif") == 0) {
    return "image/avif";
  }
  if (strcmp(extension, ".avi") == 0) {
    return "video/x-msvideo";
  }
  if (strcmp(extension, ".azw") == 0) {
    return "application/vnd.amazon.ebook";
  }
  if (strcmp(extension, ".bin") == 0) {
    return "application/octet-stream";
  }
  if (strcmp(extension, ".bmp") == 0) {
    return "image/bmp";
  }
  if (strcmp(extension, ".bz") == 0) {
    return "application/x-bzip";
  }
  if (strcmp(extension, ".bz2") == 0) {
    return "application/x-bzip2";
  }
  if (strcmp(extension, ".cda") == 0) {
    return "application/x-cdf";
  }
  if (strcmp(extension, ".csh") == 0) {
    return "application/x-csh";
  }
  if (strcmp(extension, ".csv") == 0) {
    return "text/csv";
  }
  if (strcmp(extension, ".doc") == 0) {
    return "application/msword";
  }
  if (strcmp(extension, ".docx") == 0) {
    return "application/"
           "vnd.openxmlformats-officedocument.wordprocessingml.document";
  }
  if (strcmp(extension, ".eot") == 0) {
    return "application/vnd.ms-fontobject";
  }
  if (strcmp(extension, ".epub") == 0) {
    return "application/epub+zip";
  }
  if (strcmp(extension, ".gz") == 0) {
    return "application/gzip";
  }
  if (strcmp(extension, ".ico") == 0) {
    return "image/vnd.microsoft.icon";
  }
  if (strcmp(extension, ".ics") == 0) {
    return "text/calendar";
  }
  if (strcmp(extension, ".jar") == 0) {
    return "application/java-archive";
  }
  if (strcmp(extension, ".jar") == 0) {
    return "application/java-archive";
  }
  if (strcmp(extension, ".jsonld") == 0) {
    return "application/ld+json";
  }
  if (strcmp(extension, ".mid") == 0) {
    return "audio/midi";
  }
  if (strcmp(extension, ".midi") == 0) {
    return "audio/midi";
  }
  if (strcmp(extension, ".mjs") == 0) {
    return "text/javascript";
  }
  if (strcmp(extension, ".mp3") == 0) {
    return "audio/mpeg";
  }
  if (strcmp(extension, ".mp4") == 0) {
    return "video/mp4";
  }
  if (strcmp(extension, ".mpeg") == 0) {
    return "video/mpeg";
  }
  if (strcmp(extension, ".mpkg") == 0) {
    return "video/vnd.apple.installer+xml";
  }
  if (strcmp(extension, ".odp") == 0) {
    return "application/vnd.oasis.opendocument.presentation";
  }
  if (strcmp(extension, ".ods") == 0) {
    return "application/vnd.oasis.opendocument.spreadsheet";
  }
  if (strcmp(extension, ".odt") == 0) {
    return "application/vnd.oasis.opendocument.text";
  }
  if (strcmp(extension, ".oga") == 0) {
    return "audio/ogg";
  }
  if (strcmp(extension, ".ogv") == 0) {
    return "video/ogg";
  }
  if (strcmp(extension, ".ogx") == 0) {
    return "application/ogg";
  }
  if (strcmp(extension, ".opus") == 0) {
    return "audio/ogg";
  }
  if (strcmp(extension, ".otf") == 0) {
    return "font/otf";
  }
  if (strcmp(extension, ".php") == 0) {
    return "application/x-httpd-php";
  }
  if (strcmp(extension, ".ppt") == 0) {
    return "application/vnd.ms-powerpoint";
  }
  if (strcmp(extension, ".pptx") == 0) {
    return "application/"
           "vnd.openxmlformats-officedocument.presentationml.presentation";
  }
  if (strcmp(extension, ".rar") == 0) {
    return "application/vnd.rar";
  }
  if (strcmp(extension, ".rtf") == 0) {
    return "application/rtf";
  }
  if (strcmp(extension, ".sh") == 0) {
    return "application/x-sh";
  }
  if (strcmp(extension, ".svg") == 0) {
    return "image/svg+xml";
  }
  if (strcmp(extension, ".tar") == 0) {
    return "application/x-tar";
  }
  if (strcmp(extension, ".tif") == 0) {
    return "font/tiff";
  }
  if (strcmp(extension, ".tiff") == 0) {
    return "font/tiff";
  }
  if (strcmp(extension, ".ttf") == 0) {
    return "font/ttf";
  }
  if (strcmp(extension, ".vsd") == 0) {
    return "application/vnd.visio";
  }
  if (strcmp(extension, ".wav") == 0) {
    return "audio/wav";
  }
  if (strcmp(extension, ".weba") == 0) {
    return "audio/weba";
  }
  if (strcmp(extension, ".webm") == 0) {
    return "video/webm";
  }
  if (strcmp(extension, ".webp") == 0) {
    return "image/webp";
  }
  if (strcmp(extension, ".woff") == 0) {
    return "font/woff";
  }
  if (strcmp(extension, ".woff2") == 0) {
    return "font/woff2";
  }
  if (strcmp(extension, ".xhtml") == 0) {
    return "application/xhtml+xml";
  }
  if (strcmp(extension, ".xls") == 0) {
    return "application/vnd.ms-excel";
  }
  if (strcmp(extension, ".xlsx") == 0) {
    return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  }
  if (strcmp(extension, ".xml") == 0) {
    return "application/xml";
  }
  if (strcmp(extension, ".xul") == 0) {
    return "application/vnd.mozilla.xul+xml";
  }
  if (strcmp(extension, ".zip") == 0) {
    return "application/zip";
  }
  if (strcmp(extension, ".7z") == 0) {
    return "application/x-7z-compressed";
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

  FILE *fp = fopen(filepath, "rb");
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