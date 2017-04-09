/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdlib.h>

#include "py/runtime.h"
#include "py/parsenumbase.h"
#include "py/parsenum.h"
#include "py/smallint.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include <math.h>
#endif

STATIC NORETURN void raise_exc(mp_obj_t exc, mp_lexer_t *lex) {
    // if lex!=NULL then the parser called us and we need to convert the
    // exception's type from ValueError to SyntaxError and add traceback info
    if (lex != NULL) {
        ((mp_obj_base_t*)MP_OBJ_TO_PTR(exc))->type = &mp_type_SyntaxError;
        mp_obj_exception_add_traceback(exc, lex->source_name, lex->tok_line, MP_QSTR_NULL);
    }
    nlr_raise(exc);
}

mp_obj_t mp_parse_num_integer(const char *restrict str_, size_t len, int base, mp_lexer_t *lex) {
    const byte *restrict str = (const byte *)str_;
    const byte *restrict top = str + len;
    bool neg = false;
    mp_obj_t ret_val;

    // check radix base
    if ((base != 0 && base < 2) || base > 36) {
        // this won't be reached if lex!=NULL
        mp_raise_ValueError("int() arg 2 must be >= 2 and <= 36");
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
    str += mp_parse_num_base((const char*)str, top - str, &base);

    // string should be an integer number
    mp_int_t int_val = 0;
    const byte *restrict str_val_start = str;
    for (; str < top; str++) {
        // get next digit as a value
        mp_uint_t dig = *str;
        if ('0' <= dig && dig <= '9') {
            dig -= '0';
        } else {
            dig |= 0x20; // make digit lower-case
            if ('a' <= dig && dig <= 'z') {
                dig -= 'a' - 10;
            } else {
                // unknown character
                break;
            }
        }
        if (dig >= (mp_uint_t)base) {
            break;
        }

        // add next digi and check for overflow
        if (mp_small_int_mul_overflow(int_val, base)) {
            goto overflow;
        }
        int_val = int_val * base + dig;
        if (!MP_SMALL_INT_FITS(int_val)) {
            goto overflow;
        }
    }

    // negate value if needed
    if (neg) {
        int_val = -int_val;
    }

    // create the small int
    ret_val = MP_OBJ_NEW_SMALL_INT(int_val);

have_ret_val:
    // check we parsed something
    if (str == str_val_start) {
        goto value_error;
    }

    // skip trailing space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // check we reached the end of the string
    if (str != top) {
        goto value_error;
    }

    // return the object
    return ret_val;

overflow:
    // reparse using long int
    {
        const char *s2 = (const char*)str_val_start;
        ret_val = mp_obj_new_int_from_str_len(&s2, top - str_val_start, neg, base);
        str = (const byte*)s2;
        goto have_ret_val;
    }

value_error:
    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
        mp_obj_t exc = mp_obj_new_exception_msg(&mp_type_ValueError,
            "invalid syntax for integer");
        raise_exc(exc, lex);
    } else if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL) {
        mp_obj_t exc = mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "invalid syntax for integer with base %d", base);
        raise_exc(exc, lex);
    } else {
        vstr_t vstr;
        mp_print_t print;
        vstr_init_print(&vstr, 50, &print);
        mp_printf(&print, "invalid syntax for integer with base %d: ", base);
        mp_str_print_quoted(&print, str_val_start, top - str_val_start, true);
        mp_obj_t exc = mp_obj_new_exception_arg1(&mp_type_ValueError,
            mp_obj_new_str_from_vstr(&mp_type_str, &vstr));
        raise_exc(exc, lex);
    }
}

typedef enum {
    PARSE_DEC_IN_INTG,
    PARSE_DEC_IN_FRAC,
    PARSE_DEC_IN_EXP,
} parse_dec_in_t;

mp_obj_t mp_parse_num_decimal(const char *str, size_t len, bool allow_imag, bool force_complex, mp_lexer_t *lex) {
#if MICROPY_PY_BUILTINS_FLOAT
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

    const char *str_val_start = str;

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
        parse_dec_in_t in = PARSE_DEC_IN_INTG;
        bool exp_neg = false;
        mp_float_t frac_mult = 0.1;
        mp_int_t exp_val = 0;
        while (str < top) {
            mp_uint_t dig = *str++;
            if ('0' <= dig && dig <= '9') {
                dig -= '0';
                if (in == PARSE_DEC_IN_EXP) {
                    exp_val = 10 * exp_val + dig;
                } else {
                    if (in == PARSE_DEC_IN_FRAC) {
                        dec_val += dig * frac_mult;
                        frac_mult *= MICROPY_FLOAT_CONST(0.1);
                    } else {
                        dec_val = 10 * dec_val + dig;
                    }
                }
            } else if (in == PARSE_DEC_IN_INTG && dig == '.') {
                in = PARSE_DEC_IN_FRAC;
            } else if (in != PARSE_DEC_IN_EXP && ((dig | 0x20) == 'e')) {
                in = PARSE_DEC_IN_EXP;
                if (str < top) {
                    if (str[0] == '+') {
                        str++;
                    } else if (str[0] == '-') {
                        str++;
                        exp_neg = true;
                    }
                }
                if (str == top) {
                    goto value_error;
                }
            } else if (allow_imag && (dig | 0x20) == 'j') {
                imag = true;
                break;
            } else {
                // unknown character
                str--;
                break;
            }
        }

        // work out the exponent
        if (exp_neg) {
            exp_val = -exp_val;
        }

        // apply the exponent
        dec_val *= MICROPY_FLOAT_C_FUN(pow)(10, exp_val);
    }

    // negate value if needed
    if (dec_neg) {
        dec_val = -dec_val;
    }

    // check we parsed something
    if (str == str_val_start) {
        goto value_error;
    }

    // skip trailing space
    for (; str < top && unichar_isspace(*str); str++) {
    }

    // check we reached the end of the string
    if (str != top) {
        goto value_error;
    }

    // return the object
#if MICROPY_PY_BUILTINS_COMPLEX
    if (imag) {
        return mp_obj_new_complex(0, dec_val);
    } else if (force_complex) {
        return mp_obj_new_complex(dec_val, 0);
#else
    if (imag || force_complex) {
        raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, "complex values not supported"), lex);
#endif
    } else {
        return mp_obj_new_float(dec_val);
    }

value_error:
    raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid syntax for number"), lex);

#else
    raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, "decimal numbers not supported"), lex);
#endif
}
