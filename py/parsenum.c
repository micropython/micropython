#include <stdbool.h>
#include <stdlib.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "nlr.h"
#include "obj.h"
#include "parsenumbase.h"
#include "parsenum.h"

#if MICROPY_ENABLE_FLOAT
#include <math.h>
#endif

mp_obj_t mp_parse_num_integer(const char *restrict str, uint len, int base) {
    const char *restrict top = str + len;
    bool neg = false;

    // check radix base
    if ((base != 0 && base < 2) || base > 36) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "ValueError: int() arg 2 must be >=2 and <= 36"));
    }

    // skip leading space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // parse optional sign
    if (str < top) {
        if (*str == '+') {
            str++;
        } else if (*str == '-') {
            str++;
            neg = true;
        }
    }

    // parse optional base prefix
    str += mp_parse_num_base(str, top - str, &base);

    // string should be an integer number
    machine_int_t int_val = 0;
    const char *restrict str_val_start = str;
    for (; str < top; str++) {
        machine_int_t old_val = int_val;
        int dig = *str;
        if (unichar_isdigit(dig) && dig - '0' < base) {
            // 0-9 digit
            int_val = base * int_val + dig - '0';
        } else if (base == 16) {
            dig |= 0x20;
            if ('a' <= dig && dig <= 'f') {
                // a-f hex digit
                int_val = base * int_val + dig - 'a' + 10;
            } else {
                // unknown character
                break;
            }
        } else {
            // unknown character
            break;
        }
        if (int_val < old_val) {
            // If new value became less than previous, it's overflow
            goto overflow;
        } else if ((old_val ^ int_val) & WORD_MSBIT_HIGH) {
            // If signed number changed sign - it's overflow
            goto overflow;
        }
    }

    // check we parsed something
    if (str == str_val_start) {
        goto value_error;
    }

    // negate value if needed
    if (neg) {
        int_val = -int_val;
    }

    // skip trailing space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // check we reached the end of the string
    if (str != top) {
        goto value_error;
    }

    // return the object
    return MP_OBJ_NEW_SMALL_INT(int_val);

value_error:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid literal for int() with base %d: '%s'", base, str));

overflow:
    // TODO reparse using bignum
    nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "overflow parsing integer"));
}

#define PARSE_DEC_IN_INTG (1)
#define PARSE_DEC_IN_FRAC (2)
#define PARSE_DEC_IN_EXP  (3)

mp_obj_t mp_parse_num_decimal(const char *str, uint len, bool allow_imag, bool force_complex) {
#if MICROPY_ENABLE_FLOAT
    const char *top = str + len;
    mp_float_t dec_val = 0;
    bool dec_neg = false;
    bool imag = false;

    // skip leading space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // parse optional sign
    if (str < top) {
        if (*str == '+') {
            str++;
        } else if (*str == '-') {
            str++;
            dec_neg = true;
        }
    }

    // determine what the string is
    if (str < top && (str[0] | 0x20) == 'i') {
        // string starts with 'i', should be 'inf' or 'infinity' (case insensitive)
        if (str + 2 < top && (str[1] | 0x20) == 'n' && (str[2] | 0x20) == 'f') {
            // inf
            str += 3;
            dec_val = INFINITY;
            if (str + 4 < top && (str[0] | 0x20) == 'i' && (str[1] | 0x20) == 'n' && (str[2] | 0x20) == 'i' && (str[3] | 0x20) == 't' && (str[4] | 0x20) == 'y') {
                // infinity
                str += 5;
            }
        }
    } else if (str < top && (str[0] | 0x20) == 'n') {
        // string starts with 'n', should be 'nan' (case insensitive)
        if (str + 2 < top && (str[1] | 0x20) == 'a' && (str[2] | 0x20) == 'n') {
            // NaN
            str += 3;
            dec_val = MICROPY_FLOAT_C_FUN(nan)("");
        }
    } else {
        // string should be a decimal number
        int in = PARSE_DEC_IN_INTG;
        bool exp_neg = false;
        int exp_val = 0;
        int exp_extra = 0;
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
            } else if (in != PARSE_DEC_IN_EXP && ((dig | 0x20) == 'e')) {
                in = PARSE_DEC_IN_EXP;
                if (str[1] == '+') {
                    str++;
                } else if (str[1] == '-') {
                    str++;
                    exp_neg = true;
                }
            } else if (allow_imag && (dig | 0x20) == 'j') {
                str++;
                imag = true;
                break;
            } else {
                // unknown character
                break;
            }
        }

        // work out the exponent
        if (exp_neg) {
            exp_val = -exp_val;
        }
        exp_val += exp_extra;

        // apply the exponent
        for (; exp_val > 0; exp_val--) {
            dec_val *= 10;
        }
        for (; exp_val < 0; exp_val++) {
            dec_val *= 0.1;
        }
    }

    // negate value if needed
    if (dec_neg) {
        dec_val = -dec_val;
    }

    // skip trailing space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // check we reached the end of the string
    if (str != top) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "invalid syntax for number"));
    }

    // return the object
    if (imag) {
        return mp_obj_new_complex(0, dec_val);
    } else if (force_complex) {
        return mp_obj_new_complex(dec_val, 0);
    } else {
        return mp_obj_new_float(dec_val);
    }

#else
    nlr_jump(mp_obj_new_exception_msg(&mp_type_SyntaxError, "decimal numbers not supported"));
#endif
}
