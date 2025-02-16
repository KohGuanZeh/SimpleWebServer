#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutils.h"

#define DIVIDER "========================================\n"
#define MAX_STRING_LENGTH 50

void test_split_string(void);

int main(void) {
  test_split_string();
  return 0;
}

void test_split_string(void) {
  printf(DIVIDER);
  printf("Testing split_string...\n");

  char split = 0;
  char* test_string_ptr = malloc(MAX_STRING_LENGTH * sizeof(char));
  assert(test_string_ptr != NULL);
  memset(test_string_ptr, 0, MAX_STRING_LENGTH);

  char* delim = malloc(5 * sizeof(char));
  assert(delim != NULL);

  size_t str_len = strlen("This - string - should - split.");
  strcpy(test_string_ptr, "This - string - should - split.");
  test_string_ptr[str_len] = 0;
  strcpy(delim, " - ");
  delim[3] = 0;
  printf("Splitting '%s' with '%s' as delimiter\n", test_string_ptr, delim);

  char* test_string = test_string_ptr;

  char* next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "This") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "string") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "should") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim, &split);
  assert(split == 0);
  assert(strcmp(test_string, "split.") == 0);
  assert(next_string == NULL);

  // Test for NULL pointer.
  test_string = next_string;
  next_string = split_string(test_string, delim, &split);
  assert(split == 0);
  assert(next_string == NULL);
  assert(test_string == NULL);

  memset(test_string_ptr, 0, MAX_STRING_LENGTH);
  str_len = strlen("Double -  - delimiter.");
  strcpy(test_string_ptr, "Double -  - delimiter.");
  test_string_ptr[str_len] = 0;
  printf("Splitting '%s' with '%s' as delimiter\n", test_string_ptr, delim);

  test_string = test_string_ptr;

  next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "Double") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "") == 0);
  test_string = next_string;

  next_string = split_string(test_string, delim, &split);
  assert(split == 0);
  assert(strcmp(test_string, "delimiter.") == 0);
  assert(next_string == NULL);

  memset(test_string_ptr, 0, MAX_STRING_LENGTH);
  strcpy(test_string_ptr, "p");
  test_string_ptr[1] = 0;
  strcpy(delim, "p");
  delim[1] = 0;
  printf("Splitting '%s' with '%s' as delimiter.\n", test_string_ptr, delim);

  test_string = test_string_ptr;

  next_string = split_string(test_string, delim, &split);
  assert(split == 1);
  assert(strcmp(test_string, "") == 0);
  assert(strcmp(next_string, "") == 0);

  free(test_string_ptr);
  free(delim);
  printf(DIVIDER);
  printf("PASSED\n");
  printf(DIVIDER);
}