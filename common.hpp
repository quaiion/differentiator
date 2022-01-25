#ifndef AKINATOR_COMMON_ACTIVE
#define AKINATOR_COMMON_ACTIVE

#include <stdio.h>
#include <assert.h>

#define OS_RESERVED_ADDRESS 1
#define STRINGS_EQUAL 0
#define NO_OFFSET 0

constexpr int MAX_FILE_NAME = 64;

size_t get_file_size (FILE* const file_in);
bool int_lin_search (int *arr, size_t len, int elem);

#endif