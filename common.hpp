#ifndef AKINATOR_COMMON_ACTIVE
#define AKINATOR_COMMON_ACTIVE

#include <stdio.h>
#include <assert.h>
#include <string.h>

#define OS_RESERVED_ADDRESS 1
#define STRINGS_EQUAL 0
#define NO_OFFSET 0

constexpr int MAX_FILE_NAME = 64;

size_t get_file_size (FILE* const file_in);
bool int_lin_search (int *arr, size_t len, int elem);
int str_lin_search (const char * const arr, int len, const char *elem);
int first_symb_str_lin_search (const char * const arr, int len, const char elem);
void flush_input_buffer (char symb_inserted);
int num_of_digits_llint (long long number);

#endif