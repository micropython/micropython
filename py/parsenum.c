#include <stdlib.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "nlr.h"
#include "obj.h"
#include "parsenumbase.h"
#include "parsenum.h"

#if defined(UNIX)

#include <ctype.h>
#include <errno.h>

mp_obj_t mp_parse_num_integer(const char *restrict str, uint len, int base) {
    // TODO at the moment we ignore len; we should honour it!
    // TODO detect integer overflow and return bignum

    int c, neg = 0;
    const char *p = str;
    char *num;
    long found;

    // check radix base
    if ((base != 0 && base < 2) || base > 36) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "ValueError: int() arg 2 must be >=2 and <= 36"));
    }
    // skip surrounded whitespace
    while (isspace((c = *(p++))));
    if (c == 0) {
        goto value_error;
    }
    // preced sign
    if (c == '+' || c == '-') {
        neg = - (c == '-');
    } else {
        p--;
    }

    len -= p - str;
    int skip = mp_parse_num_base(p, len, &base);
    p += skip;
    len -= skip;

    errno = 0;
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
    return MP_OBJ_NEW_SMALL_INT((found ^ neg) - neg);

value_error:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid literal for int() with base %d: '%s'", base, str));
}

#else /* defined(UNIX) */

mp_obj_t mp_parse_num_integer(const char *restrict str, uint len, int base) {
    // TODO port strtol to stm
    return MP_OBJ_NEW_SMALL_INT(0);
}

#endif /* defined(UNIX) */

#define PARSE_DEC_IN_INTG (1)
#define PARSE_DEC_IN_FRAC (2)
#define PARSE_DEC_IN_EXP  (3)

mp_obj_t mp_parse_num_decimal(const char *str, uint len) {
#if MICROPY_ENABLE_FLOAT
    int in = PARSE_DEC_IN_INTG;
    mp_float_t dec_val = 0;
    bool exp_neg = false;
    int exp_val = 0;
    int exp_extra = 0;
    bool imag = false;
    const char *top = str + len;
    for (; str < top; str++) {
        int dig = *str;
        if ('0' <= dig && dig <= '9') {
            dig -= '0';
            if (in == PARSE_DEC_IN_EXP) {
                exp_val = 10 * exp_val + dig;
            } else {
                dec_val = 10 * dec_val + dig;
                if (in == PARSE_DEC_IN_FRAC) {
                    exp_extra -= 1;
                }
            }
        } else if (in == PARSE_DEC_IN_INTG && dig == '.') {
            in = PARSE_DEC_IN_FRAC;
        } else if (in != PARSE_DEC_IN_EXP && (dig == 'E' || dig == 'e')) {
            in = PARSE_DEC_IN_EXP;
            if (str[1] == '+') {
                str++;
            } else if (str[1] == '-') {
                str++;
                exp_neg = true;
            }
        } else if (dig == 'J' || dig == 'j') {
            str++;
            imag = true;
            break;
        } else {
            // unknown character
            break;
        }
    }
    if (*str != 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "invalid syntax for number"));
    }
    if (exp_neg) {
        exp_val = -exp_val;
    }
    exp_val += exp_extra;
    for (; exp_val > 0; exp_val--) {
        dec_val *= 10;
    }
    for (; exp_val < 0; exp_val++) {
        dec_val *= 0.1;
    }
    if (imag) {
        return mp_obj_new_complex(0, dec_val);
    } else {
        return mp_obj_new_float(dec_val);
    }
#else
    nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "decimal numbers not supported"));
#endif
}
