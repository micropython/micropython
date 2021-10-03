#include <stdlib.h>

// assumes endptr != NULL
// doesn't check for sign
// doesn't check for base-prefix
long long int strtoll(const char *nptr, char **endptr, int base) {
    long long val = 0;

    for (; *nptr; nptr++) {
        int v = *nptr;
        if ('0' <= v && v <= '9') {
            v -= '0';
        } else if ('A' <= v && v <= 'Z') {
            v -= 'A' - 10;
        } else if ('a' <= v && v <= 'z') {
            v -= 'a' - 10;
        } else {
            break;
        }
        if (v >= base) {
            break;
        }
        val = val * base + v;
    }

    *endptr = (char *)nptr;

    return val;
}
