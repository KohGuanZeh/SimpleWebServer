#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutils.h"

#define DIVIDER "========================================\n"

void test_split_string(void);

int main(void) {
  test_split_string();
  return 0;
}

void test_split_string(void) {
  printf(DIVIDER);
  printf("Testing split_string...\n");

  char* test_string_ptr = malloc(50 * sizeof(char));
  assert(test_string_ptr != NULL);
  char* delim = malloc(5 * sizeof(char));
  assert(delim != NULL);

  size_t str_len = strlen("This - string - should - split.");
  strcpy(test_string_ptr, "This - string - should - split.");
  test_string_ptr[str_len] = 0;
  strcpy(delim, " - ");
  delim[3] = 0;
  printf("Splitting '%s' with '%s' as delimiter\n", test_string_ptr, delim);

  char* test_string = test_string_ptr;

  char* next_string = split_string(test_string, delim);
  assert(strcmp(test_string, "This") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim);
  assert(strcmp(test_string, "string") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim);
  assert(strcmp(test_string, "should") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim);
  assert(strcmp(test_string, "split.") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim);
  assert(next_string == NULL);

  strcpy(test_string_ptr, "p");
  test_string_ptr[1] = 0;
  strcpy(delim, "p");
  delim[1] = 0;
  printf("Splitting '%s' with '%s' as delimiter.\n", test_string_ptr, delim);

  next_string = split_string(test_string, delim);
  assert(next_string == NULL);

  free(test_string_ptr);
  free(delim);
  printf(DIVIDER);
}