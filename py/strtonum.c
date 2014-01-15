#if defined(UNIX)

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

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
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_ValueError, "ValueError: int() arg 2 must be >=2 and <= 36"));
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
    // TODO somehow merge with similar code in parse.c
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
}

#else /* defined(UNIX) */

long strtonum(const char *restrict s, int base) {
    // TODO port strtol to stm
    return 0;
}

#endif /* defined(UNIX) */
