/*
 * This file is part of the MicroPython project, http://micropython.org/
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

static NORETURN void raise_exc(mp_obj_t exc, mp_lexer_t *lex) {
    // if lex!=NULL then the parser called us and we need to convert the
    // exception's type from ValueError to SyntaxError and add traceback info
    if (lex != NULL) {
        ((mp_obj_base_t *)MP_OBJ_TO_PTR(exc))->type = &mp_type_SyntaxError;
        mp_obj_exception_add_traceback(exc, lex->source_name, lex->tok_line, MP_QSTRnull);
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
        mp_raise_ValueError(MP_ERROR_TEXT("int() arg 2 must be >= 2 and <= 36"));
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
    str += mp_parse_num_base((const char *)str, top - str, &base);

    // string should be an integer number
    mp_int_t int_val = 0;
    const byte *restrict str_val_start = str;
    for (; str < top; str++) {
        // get next digit as a value
        mp_uint_t dig = *str;
        if ('0' <= dig && dig <= '9') {
            dig -= '0';
        } else if (dig == '_') {
            continue;
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
        const char *s2 = (const char *)str_val_start;
        ret_val = mp_obj_new_int_from_str_len(&s2, top - str_val_start, neg, base);
        str = (const byte *)s2;
        goto have_ret_val;
    }

value_error:
    {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_obj_t exc = mp_obj_new_exception_msg(&mp_type_ValueError,
            MP_ERROR_TEXT("invalid syntax for integer"));
        raise_exc(exc, lex);
        #elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL
        mp_obj_t exc = mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("invalid syntax for integer with base %d"), base == 1 ? 0 : base);
        raise_exc(exc, lex);
        #else
        vstr_t vstr;
        mp_print_t print;
        vstr_init_print(&vstr, 50, &print);
        mp_printf(&print, "invalid syntax for integer with base %d: ", base == 1 ? 0 : base);
        mp_str_print_quoted(&print, str_val_start, top - str_val_start, true);
        mp_obj_t exc = mp_obj_new_exception_arg1(&mp_type_ValueError,
            mp_obj_new_str_from_utf8_vstr(&vstr));
        raise_exc(exc, lex);
        #endif
    }
}

enum {
    REAL_IMAG_STATE_START = 0,
    REAL_IMAG_STATE_HAVE_REAL = 1,
    REAL_IMAG_STATE_HAVE_IMAG = 2,
};

typedef enum {
    PARSE_DEC_IN_INTG,
    PARSE_DEC_IN_FRAC,
    PARSE_DEC_IN_EXP,
} parse_dec_in_t;

#if MICROPY_PY_BUILTINS_FLOAT
// DEC_VAL_MAX only needs to be rough and is used to retain precision while not overflowing
// SMALL_NORMAL_VAL is the smallest power of 10 that is still a normal float
// EXACT_POWER_OF_10 is the largest value of x so that 10^x can be stored exactly in a float
//   Note: EXACT_POWER_OF_10 is at least floor(log_5(2^mantissa_length)). Indeed, 10^n = 2^n * 5^n
//   so we only have to store the 5^n part in the mantissa (the 2^n part will go into the float's
//   exponent).
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define DEC_VAL_MAX 1e20F
#define SMALL_NORMAL_VAL (1e-37F)
#define SMALL_NORMAL_EXP (-37)
#define EXACT_POWER_OF_10 (9)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define DEC_VAL_MAX 1e200
#define SMALL_NORMAL_VAL (1e-307)
#define SMALL_NORMAL_EXP (-307)
#define EXACT_POWER_OF_10 (22)
#endif

// Break out inner digit accumulation routine to ease trailing zero deferral.
static void accept_digit(mp_float_t *p_dec_val, int dig, int *p_exp_extra, int in) {
    // Core routine to ingest an additional digit.
    if (*p_dec_val < DEC_VAL_MAX) {
        // dec_val won't overflow so keep accumulating
        *p_dec_val = 10 * *p_dec_val + dig;
        if (in == PARSE_DEC_IN_FRAC) {
            --(*p_exp_extra);
        }
    } else {
        // dec_val might overflow and we anyway can't represent more digits
        // of precision, so ignore the digit and just adjust the exponent
        if (in == PARSE_DEC_IN_INTG) {
            ++(*p_exp_extra);
        }
    }
}
#endif // MICROPY_PY_BUILTINS_FLOAT

#if MICROPY_PY_BUILTINS_COMPLEX
mp_obj_t mp_parse_num_decimal(const char *str, size_t len, bool allow_imag, bool force_complex, mp_lexer_t *lex)
#else
mp_obj_t mp_parse_num_float(const char *str, size_t len, bool allow_imag, mp_lexer_t *lex)
#endif
{
    #if MICROPY_PY_BUILTINS_FLOAT

    const char *top = str + len;
    mp_float_t dec_val = 0;
    bool dec_neg = false;

    #if MICROPY_PY_BUILTINS_COMPLEX
    unsigned int real_imag_state = REAL_IMAG_STATE_START;
    mp_float_t dec_real = 0;
parse_start:
    #endif

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
            dec_val = (mp_float_t)INFINITY;
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
        int exp_val = 0;
        int exp_extra = 0;
        int trailing_zeros_intg = 0, trailing_zeros_frac = 0;
        while (str < top) {
            unsigned int dig = *str++;
            if ('0' <= dig && dig <= '9') {
                dig -= '0';
                if (in == PARSE_DEC_IN_EXP) {
                    // don't overflow exp_val when adding next digit, instead just truncate
                    // it and the resulting float will still be correct, either inf or 0.0
                    // (use INT_MAX/2 to allow adding exp_extra at the end without overflow)
                    if (exp_val < (INT_MAX / 2 - 9) / 10) {
                        exp_val = 10 * exp_val + dig;
                    }
                } else {
                    if (dig == 0 || dec_val >= DEC_VAL_MAX) {
                        // Defer treatment of zeros in fractional part.  If nothing comes afterwards, ignore them.
                        // Also, once we reach DEC_VAL_MAX, treat every additional digit as a trailing zero.
                        if (in == PARSE_DEC_IN_INTG) {
                            ++trailing_zeros_intg;
                        } else {
                            ++trailing_zeros_frac;
                        }
                    } else {
                        // Time to un-defer any trailing zeros.  Intg zeros first.
                        while (trailing_zeros_intg) {
                            accept_digit(&dec_val, 0, &exp_extra, PARSE_DEC_IN_INTG);
                            --trailing_zeros_intg;
                        }
                        while (trailing_zeros_frac) {
                            accept_digit(&dec_val, 0, &exp_extra, PARSE_DEC_IN_FRAC);
                            --trailing_zeros_frac;
                        }
                        accept_digit(&dec_val, dig, &exp_extra, in);
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
            } else if (dig == '_') {
                continue;
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

        // apply the exponent, making sure it's not a subnormal value
        exp_val += exp_extra + trailing_zeros_intg;
        if (exp_val < SMALL_NORMAL_EXP) {
            exp_val -= SMALL_NORMAL_EXP;
            dec_val *= SMALL_NORMAL_VAL;
        }

        // At this point, we need to multiply the mantissa by its base 10 exponent. If possible,
        // we would rather manipulate numbers that have an exact representation in IEEE754. It
        // turns out small positive powers of 10 do, whereas small negative powers of 10 don't.
        // So in that case, we'll yield a division of exact values rather than a multiplication
        // of slightly erroneous values.
        if (exp_val < 0 && exp_val >= -EXACT_POWER_OF_10) {
            dec_val /= MICROPY_FLOAT_C_FUN(pow)(10, -exp_val);
        } else {
            dec_val *= MICROPY_FLOAT_C_FUN(pow)(10, exp_val);
        }
    }

    if (allow_imag && str < top && (*str | 0x20) == 'j') {
        #if MICROPY_PY_BUILTINS_COMPLEX
        if (str == str_val_start) {
            // Convert "j" to "1j".
            dec_val = 1;
        }
        ++str;
        real_imag_state |= REAL_IMAG_STATE_HAVE_IMAG;
        #else
        raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, MP_ERROR_TEXT("complex values not supported")), lex);
        #endif
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
        #if MICROPY_PY_BUILTINS_COMPLEX
        if (force_complex && real_imag_state == REAL_IMAG_STATE_START) {
            // If we've only seen a real so far, keep parsing for the imaginary part.
            dec_real = dec_val;
            dec_val = 0;
            real_imag_state |= REAL_IMAG_STATE_HAVE_REAL;
            goto parse_start;
        }
        #endif
        goto value_error;
    }

    #if MICROPY_PY_BUILTINS_COMPLEX
    if (real_imag_state == REAL_IMAG_STATE_HAVE_REAL) {
        // We're on the second part, but didn't get the expected imaginary number.
        goto value_error;
    }
    #endif

    // return the object

    #if MICROPY_PY_BUILTINS_COMPLEX
    if (real_imag_state != REAL_IMAG_STATE_START) {
        return mp_obj_new_complex(dec_real, dec_val);
    } else if (force_complex) {
        return mp_obj_new_complex(dec_val, 0);
    }
    #endif

    return mp_obj_new_float(dec_val);

value_error:
    raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, MP_ERROR_TEXT("invalid syntax for number")), lex);

    #else
    raise_exc(mp_obj_new_exception_msg(&mp_type_ValueError, MP_ERROR_TEXT("decimal numbers not supported")), lex);
    #endif
}
