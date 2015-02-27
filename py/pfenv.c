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

#include <stdint.h>
#include <string.h>

#include "py/objint.h"
#include "py/pfenv.h"

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#include <stdio.h>
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#include "py/formatfloat.h"
#endif

static const char pad_spaces[] = "                ";
static const char pad_zeroes[] = "0000000000000000";

void pfenv_vstr_add_strn(void *data, const char *str, mp_uint_t len){
    vstr_add_strn(data, str, len);
}

int pfenv_print_strn(const pfenv_t *pfenv, const char *str, mp_uint_t len, int flags, char fill, int width) {
    int left_pad = 0;
    int right_pad = 0;
    int pad = width - len;
    int pad_size;
    int total_chars_printed = 0;
    const char *pad_chars;

    if (!fill || fill == ' ' ) {
        pad_chars = pad_spaces;
        pad_size = sizeof(pad_spaces) - 1;
    } else if (fill == '0') {
        pad_chars = pad_zeroes;
        pad_size = sizeof(pad_zeroes) - 1;
    } else {
        // Other pad characters are fairly unusual, so we'll take the hit
        // and output them 1 at a time.
        pad_chars = &fill;
        pad_size = 1;
    }

    if (flags & PF_FLAG_CENTER_ADJUST) {
        left_pad = pad / 2;
        right_pad = pad - left_pad;
    } else if (flags & PF_FLAG_LEFT_ADJUST) {
        right_pad = pad;
    } else {
        left_pad = pad;
    }

    if (left_pad > 0) {
        total_chars_printed += left_pad;
        while (left_pad > 0) {
            int p = left_pad;
            if (p > pad_size) {
                p = pad_size;
            }
            pfenv->print_strn(pfenv->data, pad_chars, p);
            left_pad -= p;
        }
    }
    if (len) {
        pfenv->print_strn(pfenv->data, str, len);
        total_chars_printed += len;
    }
    if (right_pad > 0) {
        total_chars_printed += right_pad;
        while (right_pad > 0) {
            int p = right_pad;
            if (p > pad_size) {
                p = pad_size;
            }
            pfenv->print_strn(pfenv->data, pad_chars, p);
            right_pad -= p;
        }
    }
    return total_chars_printed;
}

// 32-bits is 10 digits, add 3 for commas, 1 for sign, 1 for terminating null
// We can use 16 characters for 32-bit and 32 characters for 64-bit
#define INT_BUF_SIZE (sizeof(mp_int_t) * 4)

// This function is used by stmhal port to implement printf.
// It needs to be a separate function to pfenv_print_mp_int, since converting to a mp_int looses the MSB.
int pfenv_print_int(const pfenv_t *pfenv, mp_uint_t x, int sgn, int base, int base_char, int flags, char fill, int width) {
    char sign = 0;
    if (sgn) {
        if ((mp_int_t)x < 0) {
            sign = '-';
            x = -x;
        } else if (flags & PF_FLAG_SHOW_SIGN) {
            sign = '+';
        } else if (flags & PF_FLAG_SPACE_SIGN) {
            sign = ' ';
        }
    }

    char buf[INT_BUF_SIZE];
    char *b = buf + INT_BUF_SIZE;

    if (x == 0) {
        *(--b) = '0';
    } else {
        do {
            int c = x % base;
            x /= base;
            if (c >= 10) {
                c += base_char - 10;
            } else {
                c += '0';
            }
            *(--b) = c;
        } while (b > buf && x != 0);
    }

    char prefix_char = '\0';

    if (flags & PF_FLAG_SHOW_PREFIX) {
        if (base == 2) {
            prefix_char = base_char + 'b' - 'a';
        } else if (base == 8) {
            prefix_char = base_char + 'o' - 'a';
        } else if (base == 16) {
            prefix_char = base_char + 'x' - 'a';
        }
    }

    int len = 0;
    if (flags & PF_FLAG_PAD_AFTER_SIGN) {
        if (sign) {
            len += pfenv_print_strn(pfenv, &sign, 1, flags, fill, 1);
            width--;
        }
        if (prefix_char) {
            len += pfenv_print_strn(pfenv, "0", 1, flags, fill, 1);
            len += pfenv_print_strn(pfenv, &prefix_char, 1, flags, fill, 1);
            width -= 2;
        }
    } else {
        if (prefix_char && b > &buf[1]) {
            *(--b) = prefix_char;
            *(--b) = '0';
        }
        if (sign && b > buf) {
            *(--b) = sign;
        }
    }

    len += pfenv_print_strn(pfenv, b, buf + INT_BUF_SIZE - b, flags, fill, width);
    return len;
}

int pfenv_print_mp_int(const pfenv_t *pfenv, mp_obj_t x, int sgn, int base, int base_char, int flags, char fill, int width, int prec) {
    (void)sgn; // TODO why is sgn unused?

    if (!MP_OBJ_IS_INT(x)) {
        // This will convert booleans to int, or raise an error for
        // non-integer types.
        x = MP_OBJ_NEW_SMALL_INT(mp_obj_get_int(x));
    }

    if ((flags & (PF_FLAG_LEFT_ADJUST | PF_FLAG_CENTER_ADJUST)) == 0 && fill == '0') {
        if (prec > width) {
            width = prec;
        }
        prec = 0;
    }
    char prefix_buf[4];
    char *prefix = prefix_buf;

    if (mp_obj_int_is_positive(x)) {
        if (flags & PF_FLAG_SHOW_SIGN) {
            *prefix++ = '+';
        } else if (flags & PF_FLAG_SPACE_SIGN) {
            *prefix++ = ' ';
        }
    }

    if (flags & PF_FLAG_SHOW_PREFIX) {
        if (base == 2) {
            *prefix++ = '0';
            *prefix++ = base_char + 'b' - 'a';
        } else if (base == 8) {
            *prefix++ = '0';
            if (flags & PF_FLAG_SHOW_OCTAL_LETTER) {
                *prefix++ = base_char + 'o' - 'a';
            }
        } else if (base == 16) {
            *prefix++ = '0';
            *prefix++ = base_char + 'x' - 'a';
        }
    }
    *prefix = '\0';
    int prefix_len = prefix - prefix_buf;
    prefix = prefix_buf;

    char comma = '\0';
    if (flags & PF_FLAG_SHOW_COMMA) {
        comma = ',';
    }

    // The size of this buffer is rather arbitrary. If it's not large
    // enough, a dynamic one will be allocated.
    char stack_buf[sizeof(mp_int_t) * 4];
    char *buf = stack_buf;
    mp_uint_t buf_size = sizeof(stack_buf);
    mp_uint_t fmt_size = 0;
    char *str;

    if (prec > 1) {
        flags |= PF_FLAG_PAD_AFTER_SIGN;
    }
    char sign = '\0';
    if (flags & PF_FLAG_PAD_AFTER_SIGN) {
        // We add the pad in this function, so since the pad goes after
        // the sign & prefix, we format without a prefix
        str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size,
                                   x, base, NULL, base_char, comma);
        if (*str == '-') {
            sign = *str++;
            fmt_size--;
        }
    } else {
        str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size,
                                   x, base, prefix, base_char, comma);
    }

    int spaces_before = 0;
    int spaces_after = 0;

    if (prec > 1) {
        // If prec was specified, then prec specifies the width to zero-pad the
        // the number to. This zero-padded number then gets left or right
        // aligned in width characters.

        int prec_width = fmt_size;  // The digits
        if (prec_width < prec) {
            prec_width = prec;
        }
        if (flags & PF_FLAG_PAD_AFTER_SIGN) {
            if (sign) {
                prec_width++;
            }
            prec_width += prefix_len;
        }
        if (prec_width < width) {
            if (flags & PF_FLAG_LEFT_ADJUST) {
                spaces_after = width - prec_width;
            } else {
                spaces_before = width - prec_width;
            }
        }
        fill = '0';
        flags &= ~PF_FLAG_LEFT_ADJUST;
    }

    int len = 0;
    if (spaces_before) {
        len += pfenv_print_strn(pfenv, "", 0, 0, ' ', spaces_before);
    }
    if (flags & PF_FLAG_PAD_AFTER_SIGN) {
        // pad after sign implies pad after prefix as well.
        if (sign) {
            len += pfenv_print_strn(pfenv, &sign, 1, 0, 0, 1);
            width--;
        }
        if (prefix_len) {
            len += pfenv_print_strn(pfenv, prefix, prefix_len, 0, 0, 1);
            width -= prefix_len;
        }
    }
    if (prec > 1) {
        width = prec;
    }

    len += pfenv_print_strn(pfenv, str, fmt_size, flags, fill, width);

    if (spaces_after) {
        len += pfenv_print_strn(pfenv, "", 0, 0, ' ', spaces_after);
    }

    if (buf != stack_buf) {
        m_del(char, buf, buf_size);
    }
    return len;
}

#if MICROPY_PY_BUILTINS_FLOAT
int pfenv_print_float(const pfenv_t *pfenv, mp_float_t f, char fmt, int flags, char fill, int width, int prec) {
    char buf[32];
    char sign = '\0';
    int chrs = 0;

    if (flags & PF_FLAG_SHOW_SIGN) {
        sign = '+';
    }
    else
    if (flags & PF_FLAG_SPACE_SIGN) {
        sign = ' ';
    }
    int len;
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    len = mp_format_float(f, buf, sizeof(buf), fmt, prec, sign);
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
    char fmt_buf[6];
    char *fmt_s = fmt_buf;

    *fmt_s++ = '%';
    if (sign) {
        *fmt_s++ = sign;
    }
    *fmt_s++ = '.';
    *fmt_s++ = '*';
    *fmt_s++ = fmt;
    *fmt_s = '\0';

    len = snprintf(buf, sizeof(buf), fmt_buf, prec, f);
    if (len < 0) {
        len = 0;
    }
#else
#error Unknown MICROPY FLOAT IMPL
#endif
    char *s = buf;

    if ((flags & PF_FLAG_ADD_PERCENT) && (size_t)(len + 1) < sizeof(buf)) {
        buf[len++] = '%';
        buf[len] = '\0';
    }

    // buf[0] < '0' returns true if the first character is space, + or -
    if ((flags & PF_FLAG_PAD_AFTER_SIGN) && buf[0] < '0') {
        // We have a sign character
        s++;
        if (*s <= '9' || (flags & PF_FLAG_PAD_NAN_INF)) {
            // We have a number, or we have a inf/nan and PAD_NAN_INF is set
            // With '{:06e}'.format(float('-inf')) you get '-00inf'
            chrs += pfenv_print_strn(pfenv, &buf[0], 1, 0, 0, 1);
            width--;
            len--;
        }
    }

    if (*s > 'A' && (flags & PF_FLAG_PAD_NAN_INF) == 0) {
        // We have one of the inf or nan variants, suppress zero fill.
        // With printf, if you use: printf("%06e", -inf) then you get "  -inf"
        // so suppress the zero fill.
        fill = ' ';
    }
    chrs += pfenv_print_strn(pfenv, s, len, flags, fill, width);

    return chrs;
}
#endif
