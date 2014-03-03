#include "misc.h"
#include "mpconfig.h"
#include "parsenumbase.h"

// find real radix base, and strip preceding '0x', '0o' and '0b'
// puts base in *base, and returns number of bytes to skip the prefix
int mp_parse_num_base(const char *str, uint len, int *base) {
    const char *p = str;
    int c = *(p++);
    if ((*base == 0 || *base == 16) && c == '0') {
        c = *(p++);
        if ((c | 32) == 'x') {
            *base = 16;
        } else if (*base == 0 && (c | 32) == 'o') {
            *base = 8;
        } else if (*base == 0 && (c | 32) == 'b') {
            *base = 2;
        } else {
            *base = 10;
            p -= 2;
        }
    } else if (*base == 8 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'o') {
            p -= 2;
        }
    } else if (*base == 2 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'b') {
            p -= 2;
        }
    } else {
        if (*base == 0) {
            *base = 10;
        }
        p--;
    }
    return p - str;
}

