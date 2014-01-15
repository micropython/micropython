#if defined(UNIX)

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "nlr.h"
#include "obj.h"


long strtonum(const char *restrict s, int base) {
    int c, neg = 0;
    const char *p = s;
    char *num;
    long found;

    // check radix base
    if ((base != 0 && base < 2) || base > 36) {
        printf("ValueError: int() arg 2 must be >=2 and <= 36\n");
        return 0;
    }
    // skip surrounded whitespace
    while (isspace((c = *(p++))));
    if (c == 0) {
        goto value_error;
    }
    // preced sign
    if (c == '+' || c == '-') {
        neg = - (c == '-');
        c = *(p++);
    }

    // find real radix base, and strip preced '0x', '0o' and '0b'
    if ((base == 0 || base == 16) && c == '0') {
        c = *(p++);
        if ((c | 32) == 'x') {
            base = 16;
        } else if (base == 0 && (c | 32) == 'o') {
            base = 8;
        } else if (base == 0 && (c | 32) == 'b') {
            base = 2;
        } else {
            base = 10;
            p -= 2;
        }
    } else if (base == 8 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'o') {
            p -= 2;
        }
    } else if (base == 2 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'b') {
            p -= 2;
        }
    } else {
        if (base == 0) base = 10;
        p--;
    }

    found = strtol(p, &num, base);
    if (errno) {
        goto value_error;
    } else if (found && *(num) == 0) {
        goto done;
    } else if (found || num != p) {
        goto check_tail_space;
    } else {
        goto value_error;
    }

check_tail_space:
    if (*(num) != 0) {
        while (isspace((c = *(num++))));
        if (c != 0) {
            goto value_error;
        }
    }

done:
    return (found ^ neg) - neg;

value_error:
    nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_ValueError, "invalid literal for int() with base %d: '%s'", (void*)(machine_uint_t)base, s));
    //printf("invalid literal for int() with base %d: '%s'\n", base, s);
    //return 0;
}

/*
int main(void) {
    printf("  0 %ld\n", strtonum("0", 0));
    printf("  1 %ld\n", strtonum("1", 0));
    printf("  1 %ld\n", strtonum("+1", 0));
    printf(" -1 %ld\n", strtonum("-1", 0));
    printf("  0 %ld\n", strtonum("+0", 0));
    printf("  0 %ld\n", strtonum("-0", 0));
    printf("  1 %ld\n", strtonum("01", 0));
    printf("  9 %ld\n", strtonum("9", 0));
    printf(" 10 %ld\n", strtonum("10", 0));
    printf(" 10 %ld\n", strtonum("+10", 0));
    printf("-10 %ld\n", strtonum("-10", 0));
    printf(" 12 %ld\n", strtonum("12", 0));
    printf("-12 %ld\n", strtonum("-12", 0));
    printf(" 99 %ld\n", strtonum("99", 0));
    printf("100 %ld\n", strtonum("100", 0));
    printf("314 %ld\n", strtonum("314", 0));
    printf("314 %ld\n", strtonum(" 314", 0));
    printf("314 %ld\n", strtonum("314 ", 0));
    printf("314 %ld\n", strtonum("  \t\t 314 \t\t  ", 0));
    printf("  1 %ld\n", strtonum(" 1 ", 0));
    printf(" -3 %ld\n", strtonum(" -3 ", 0));
    printf(" 83 %ld\n", strtonum("0o123", 0));
    printf(" 18 %ld\n", strtonum("0x12", 0));
    printf(" 10 %ld\n", strtonum("0o12", 0));
    printf(" 10 %ld\n", strtonum("0o12", 0));
    printf(" 10 %ld\n", strtonum("  \t 0o12", 0));
    printf(" 10 %ld\n", strtonum("0o12   \t  ", 0));
    printf(" 10 %ld\n", strtonum("0O12", 0));
    printf("  5 %ld\n", strtonum("0b101", 0));
    printf("  5 %ld\n", strtonum("0b101", 0));

    printf("  0 %ld\n", strtonum("0", 10));
    printf("  1 %ld\n", strtonum("1", 10));
    printf("  1 %ld\n", strtonum(" \t 1 \t ", 10));
    printf(" 11 %ld\n", strtonum("11", 10));
    printf(" 17 %ld\n", strtonum("11", 16));
    printf("  9 %ld\n", strtonum("11", 8));
    printf("  3 %ld\n", strtonum("11", 2));
    printf(" 37 %ld\n", strtonum("11", 36));
    printf("4294967296 %ld\n", strtonum("4294967296", 0));

    printf("%ld\n", strtonum("x", 0));
    printf("%ld\n", strtonum("1x", 0));
    printf("%ld\n", strtonum("  1x", 0));
    printf("%ld\n", strtonum("  1\02  ", 0));
    printf("%ld\n", strtonum("", 0));
    printf("%ld\n", strtonum(" ", 0));
    printf("%ld\n", strtonum(" \t\t  ", 0));
    printf("%ld\n", strtonum("0x", 0));
    printf("%ld\n", strtonum("0x", 16));
    printf("%ld\n", strtonum("0o", 0));
    printf("%ld\n", strtonum("0o", 8));
    printf("%ld\n", strtonum("0b", 0));
    printf("%ld\n", strtonum("0b", 2));
    printf("%ld\n", strtonum("0b2", 2));
    printf("%ld\n", strtonum("0o8", 8));
    printf("%ld\n", strtonum("0xg", 16));

    printf("%ld\n", strtonum("1 1", 0));
    printf("%ld\n", strtonum("18446744073709551616", 0));
}
*/

#else /* defined(UNIX) */

long strtonum(const char *restrict s, int base) {
    // TODO port strtol to stm
    return 0;
}

#endif /* defined(UNIX) */
