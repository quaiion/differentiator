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

        if (arr[i] == elem) {

            return i;
        }
    }

    return -1;
}
