#include "common.hpp"

static long long eu_mod (long long val_1, long long val_2);

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

int str_lin_search (const char * const *arr, int len, const char *elem) {

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

int first_symb_str_lin_search (const char * const *arr, int len, const char elem) {

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

long long int_abs (long long val) { return (val < 0) ? -val : val; }

static long long eu_mod (long long val_1, long long val_2) {

    assert (val_2 != 0);

    long long r = val_1 % val_2;
    if (r < 0) {
        
        r += int_abs (val_2);
    }

    return r;
}

long long find_gcd (long long val_1, long long val_2) {

    assert (val_2 != 0);

    long long mod = eu_mod (val_1, val_2);
    while (mod != 0) {

        val_1 = val_2;
        val_2 = mod;
        mod = eu_mod (val_1, val_2);
    }

    return val_2;
}

long long find_positive_pow (long long bas, long long pow) {

    assert (pow >= 0);

    long long res = 1;
    for (int i = 0; i < pow; ++i) {

        res *= bas;
    }

    return res;
}
