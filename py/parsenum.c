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
#include "py/mpz.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include <math.h>
#endif

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ
#define USE_EXACT_PARSER (1)
#else
#define USE_EXACT_PARSER (0)
#endif

static MP_NORETURN void raise_exc(mp_obj_t exc, mp_lexer_t *lex) {
    // if lex!=NULL then the parser called us and we need to convert the
    // exception's type from ValueError to SyntaxError and add traceback info
    if (lex != NULL) {
        ((mp_obj_base_t *)MP_OBJ_TO_PTR(exc))->type = &mp_type_SyntaxError;
        mp_obj_exception_add_traceback(exc, lex->source_name, lex->tok_line, MP_QSTRnull);
    }
    nlr_raise(exc);
}

#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_LONGLONG
// For the common small integer parsing case, we parse directly to mp_int_t and
// check that the value doesn't overflow a smallint (in which case we fail over
// to bigint parsing if supported)
typedef mp_int_t parsed_int_t;

#define PARSED_INT_MUL_OVERFLOW mp_small_int_mul_overflow
#define PARSED_INT_FITS MP_SMALL_INT_FITS
#else
// In the special case where bigint support is long long, we save code size by
// parsing directly to long long and then return either a bigint or smallint
// from the same result.
//
// To avoid pulling in (slow) signed 64-bit math routines we do the initial
// parsing to an unsigned long long and only convert to signed at the end.
typedef unsigned long long parsed_int_t;

#define PARSED_INT_MUL_OVERFLOW mp_mul_ull_overflow
#define PARSED_INT_FITS(I) ((I) <= (unsigned long long)LLONG_MAX)
#endif

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
    parsed_int_t parsed_val = 0;
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

        // add next digit and check for overflow
        if (PARSED_INT_MUL_OVERFLOW(parsed_val, base, &parsed_val)) {
            goto overflow;
        }
        parsed_val += dig;
        if (!PARSED_INT_FITS(parsed_val)) {
            goto overflow;
        }
    }

    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_LONGLONG
    // The PARSED_INT_FITS check above ensures parsed_val fits in small int representation
    ret_val = MP_OBJ_NEW_SMALL_INT(neg ? (-parsed_val) : parsed_val);
have_ret_val:
    #else
    // The PARSED_INT_FITS check above ensures parsed_val won't overflow signed long long
    long long signed_val = parsed_val;
    if (neg) {
        signed_val = -signed_val;
    }
    ret_val = mp_obj_new_int_from_ll(signed_val); // Could be large or small int
    #endif

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
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_LONGLONG
    // reparse using long int
    {
        const char *s2 = (const char *)str_val_start;
        ret_val = mp_obj_new_int_from_str_len(&s2, top - str_val_start, neg, base);
        str = (const byte *)s2;
        goto have_ret_val;
    }
    #else
    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("result overflows long long storage"));
    #endif

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

#if MICROPY_PY_BUILTINS_FLOAT

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

#if USE_EXACT_PARSER

static int mp_parse_decimal_exact(const char **str_in, const char *top, bool allow_imag, mp_float_t *float_out) {
    const char *str = *str_in;

    // TODO try to use fixed-allocated mpz on the stack
    mpz_t mpz_tmp1, mpz_tmp2;
    mpz_init_from_int(&mpz_tmp1, 10);
    mpz_init_zero(&mpz_tmp2);

    mpz_t dec;
    mpz_init_zero(&dec);

    int ret = 0;
    int exp_extra = 0;
    int exp_val = 0;
    int exp_sign = 1;
    parse_dec_in_t in = PARSE_DEC_IN_INTG;

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
                if (mpz_max_num_bits(&dec) < 52 + MPZ_DIG_SIZE) {
                    // Can possibly represent more digits so accumulate them
                    mpz_set_from_int(&mpz_tmp2, dig);
                    mpz_mul_inpl(&dec, &dec, &mpz_tmp1);
                    mpz_add_inpl(&dec, &dec, &mpz_tmp2);
                    if (in == PARSE_DEC_IN_FRAC) {
                        --exp_extra;
                    }
                } else {
                    // Can't represent more digits of precision so ignore the digit and
                    // just adjust the exponent
                    if (in == PARSE_DEC_IN_INTG) {
                        ++exp_extra;
                    }
                }
            }
        } else if (in == PARSE_DEC_IN_INTG && dig == '.') {
            in = PARSE_DEC_IN_FRAC;
        } else if (in != PARSE_DEC_IN_EXP && ((dig | 0x20) == 'e')) {
            in = PARSE_DEC_IN_EXP;
            if (str < top) {
                if (str[0] == '+') {
                    ++str;
                } else if (str[0] == '-') {
                    ++str;
                    exp_sign = -1;
                }
            }
            if (str == top) {
                ret = -1;
                goto cleanup;
            }
        } else if (allow_imag && (dig | 0x20) == 'j') {
            --str;
            break;
        } else if (dig == '_') {
            continue;
        } else {
            // unknown character
            --str;
            break;
        }
    }

    *str_in = str;

    // special case
    if (mpz_is_zero(&dec)) {
        *float_out = 0.0;
        goto cleanup;
    }

    exp_val *= exp_sign;
    exp_val += exp_extra;

    // Catch very large exponents, because 5**abs(exp_val) would be impossible to compute
    // TODO make this threshold precise, based on size of dec
    if (exp_val < -400) {
        *float_out = 0.0;
        goto cleanup;
    } else if (exp_val > 400) {
        *float_out = (mp_float_t)INFINITY;
        goto cleanup;
    }

    // Compute: 5 ** abs(exp_val)
    mpz_t mpz_exp5;
    mpz_init_zero(&mpz_exp5);
    mpz_init_from_int(&mpz_tmp1, 5);
    mpz_init_from_int(&mpz_tmp2, abs(exp_val));
    mpz_pow_inpl(&mpz_exp5, &mpz_tmp1, &mpz_tmp2);

    if (exp_val >= 0) {
        mpz_mul_inpl(&dec, &dec, &mpz_exp5);
    } else {
        // dec <<= 3 * (-exp_val) + 54
        mpz_shl_inpl(&dec, &dec, 3 * (-exp_val) + 54);

        // dec /= 5 ** (-exp_val)
        mpz_set(&mpz_tmp2, &dec);
        mpz_divmod_inpl(&dec, &mpz_tmp1, &mpz_tmp2, &mpz_exp5);

        // adjust exponent, only power of 2 left
        exp_val += 3 * exp_val - 54;
    }

    // normalise so bit 52 of mantissa is 1 (need 2 extra bits for rounding later on)
    // TODO make this much more efficient, not using 2 loops!
    mpz_set_from_int(&mpz_tmp1, 1);
    mpz_shl_inpl(&mpz_tmp1, &mpz_tmp1, 54);
    #if 0
    // Only needed if we want to use the mpz bits to create the FP bits
    while (mpz_cmp(&dec, &mpz_tmp1) < 0) {
        exp_val -= 1;
        mpz_shl_inpl(&dec, &dec, 1);
    }
    #endif
    mpz_shl_inpl(&mpz_tmp1, &mpz_tmp1, 1);
    while (mpz_cmp(&dec, &mpz_tmp1) > 0) {
        exp_val += 1;
        mpz_dig_t carry = dec.dig[0] & 1;
        mpz_shr_inpl(&dec, &dec, 1);
        dec.dig[0] |= carry;
    }

    // Looks ok to just reuse mpz_as_float to do the final conversion
    // (this will be the only conversion with possible error, we are allow one error)
    mp_float_t fdec = mpz_as_float(&dec);

    // This code computes the (double) representation exactly from the big-int bits
    /*
    dec >>= 1
    if dec & 1:
        dec += 1
    dec >>= 1
    dec_bytes = bytearray(dec.to_bytes(8, 'little'))

    # compute exponent
    fexp = 54 + 1023
    dec_bytes[6] |= fexp << 4 & 0xff
    dec_bytes[7] |= fexp >> 4

    fdec = array.array('d', dec_bytes)[0]
    */

    // ldexp is only needed to handle subnormals, otherwise fdec * 2**exp_val would suffice
    *float_out = MICROPY_FLOAT_C_FUN(ldexp)(fdec, exp_val);

cleanup:
    mpz_deinit(&dec);
    mpz_deinit(&mpz_tmp1);
    mpz_deinit(&mpz_tmp2);
    return ret;
}

#else

// MANTISSA_MAX is used to retain precision while not overflowing mantissa
// SMALL_NORMAL_VAL is the smallest power of 10 that is still a normal float
// EXACT_POWER_OF_10 is the largest value of x so that 10^x can be stored exactly in a float
//   Note: EXACT_POWER_OF_10 is at least floor(log_5(2^mantissa_length)). Indeed, 10^n = 2^n * 5^n
//   so we only have to store the 5^n part in the mantissa (the 2^n part will go into the float's
//   exponent).
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MANTISSA_MAX 0x19999998U
#define SMALL_NORMAL_VAL (1e-37F)
#define SMALL_NORMAL_EXP (-37)
#define EXACT_POWER_OF_10 (9)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MANTISSA_MAX 0x1999999999999998ULL
#define SMALL_NORMAL_VAL (1e-307)
#define SMALL_NORMAL_EXP (-307)
#define EXACT_POWER_OF_10 (22)
#endif

// Break out inner digit accumulation routine to ease trailing zero deferral.
static mp_float_uint_t accept_digit(mp_float_uint_t p_mantissa, unsigned int dig, int *p_exp_extra, int in) {
    // Core routine to ingest an additional digit.
    if (p_mantissa < MANTISSA_MAX) {
        // dec_val won't overflow so keep accumulating
        if (in == PARSE_DEC_IN_FRAC) {
            --(*p_exp_extra);
        }
        return 10u * p_mantissa + dig;
    } else {
        // dec_val might overflow and we anyway can't represent more digits
        // of precision, so ignore the digit and just adjust the exponent
        if (in == PARSE_DEC_IN_INTG) {
            ++(*p_exp_extra);
        }
        return p_mantissa;
    }
}

#endif // USE_EXACT_PARSER

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

    #if MICROPY_PY_BUILTINS_COMPLEX
    unsigned int real_imag_state = REAL_IMAG_STATE_START;
    mp_float_t dec_real = 0;
parse_start:;
    #endif
    bool dec_neg = false;

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
    if (str + 2 < top && (str[0] | 0x20) == 'i' && (str[1] | 0x20) == 'n' && (str[2] | 0x20) == 'f') {
        // 'inf' or 'infinity' (case insensitive)
        str += 3;
        dec_val = (mp_float_t)INFINITY;
        if (str + 4 < top && (str[0] | 0x20) == 'i' && (str[1] | 0x20) == 'n' && (str[2] | 0x20) == 'i' && (str[3] | 0x20) == 't' && (str[4] | 0x20) == 'y') {
            // infinity
            str += 5;
        }
    } else if (str + 2 < top && (str[0] | 0x20) == 'n' && (str[1] | 0x20) == 'a' && (str[2] | 0x20) == 'n') {
        // 'nan' (case insensitive)
        str += 3;
        dec_val = MICROPY_FLOAT_C_FUN(nan)("");
    } else {
        // string should be a decimal number
        #if USE_EXACT_PARSER
        if (mp_parse_decimal_exact(&str, top, allow_imag, &dec_val)) {
            goto value_error;
        }
        #else
        parse_dec_in_t in = PARSE_DEC_IN_INTG;
        bool exp_neg = false;
        mp_float_uint_t mantissa = 0;
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
                    if (dig == 0 || mantissa >= MANTISSA_MAX) {
                        // Defer treatment of zeros in fractional part.  If nothing comes afterwards, ignore them.
                        // Also, once we reach MANTISSA_MAX, treat every additional digit as a trailing zero.
                        if (in == PARSE_DEC_IN_INTG) {
                            ++trailing_zeros_intg;
                        } else {
                            ++trailing_zeros_frac;
                        }
                    } else {
                        // Time to un-defer any trailing zeros.  Intg zeros first.
                        while (trailing_zeros_intg) {
                            mantissa = accept_digit(mantissa, 0, &exp_extra, PARSE_DEC_IN_INTG);
                            --trailing_zeros_intg;
                        }
                        while (trailing_zeros_frac) {
                            mantissa = accept_digit(mantissa, 0, &exp_extra, PARSE_DEC_IN_FRAC);
                            --trailing_zeros_frac;
                        }
                        mantissa = accept_digit(mantissa, dig, &exp_extra, in);
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
        dec_val = (mp_float_t)mantissa;
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
        #endif
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
