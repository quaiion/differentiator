#include "common.hpp"

size_t get_file_size (FILE* const file_in) {

    assert (file_in);

    fseek (file_in, NO_OFFSET, SEEK_END);

    size_t filesize = (size_t) ftell (file_in);

    rewind (file_in);
    return filesize;
}

int int_lin_search (const int *arr, int len, int elem) {

    assert (arr);
    assert (len >= 0);

    for (int i = 0; i < len; ++i) {

        if (arr [i] == elem) {

            return i;
        }
    }

    return -1;
}

int str_lin_search (const char * const arr, int len, const char *elem) {

    assert (arr);
    assert (len >= 0);
    assert (elem);

    for (int i = 0; i < len; ++i) {

        if (strcmp (arr [i], elem) == STRINGS_EQUAL) {

            return i;
        }
    }

    return -1;
}

int first_symb_str_lin_search (const char * const arr, int len, const char elem) {

    assert (arr);
    assert (len >= 0);

    for (int i = 0; i < len; ++i) {

        if (arr [i][0] == elem) {

            return i;
        }
    }

    return -1;
}

int num_of_digits_llint (long long number) {

    int num_of_digits = 0;

    if (number < 0) {

        num_of_digits = 1;
    }

    do {

        number /= 10;
        num_of_digits += 1;

    } while (number != 0);

    return num_of_digits;
}

void flush_input_buffer (char symb_inserted) {

    if (symb_inserted != '\n') {

        while (getchar () != '\n') ;
    }
}
