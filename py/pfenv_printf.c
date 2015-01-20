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

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "py/pfenv.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include "py/formatfloat.h"
#endif

int pfenv_vprintf(const pfenv_t *pfenv, const char *fmt, va_list args) {
    int chrs = 0;
    for (;;) {
        {
            const char *f = fmt;
            while (*f != '\0' && *f != '%') {
                ++f; // XXX UTF8 advance char
            }
            if (f > fmt) {
                pfenv->print_strn(pfenv->data, fmt, f - fmt);
                chrs += f - fmt;
                fmt = f;
            }
        }

        if (*fmt == '\0') {
            break;
        }

        // move past % character
        ++fmt;

        // parse flags, if they exist
        int flags = 0;
        char fill = ' ';
        while (*fmt != '\0') {
            if (*fmt == '-') flags |= PF_FLAG_LEFT_ADJUST;
            else if (*fmt == '+') flags |= PF_FLAG_SHOW_SIGN;
            else if (*fmt == ' ') flags |= PF_FLAG_SPACE_SIGN;
            else if (*fmt == '!') flags |= PF_FLAG_NO_TRAILZ;
            else if (*fmt == '0') {
                flags |= PF_FLAG_PAD_AFTER_SIGN;
                fill = '0';
            } else break;
            ++fmt;
        }

        // parse width, if it exists
        int width = 0;
        for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
            width = width * 10 + *fmt - '0';
        }

        // parse precision, if it exists
        int prec = -1;
        if (*fmt == '.') {
            ++fmt;
            if (*fmt == '*') {
                ++fmt;
                prec = va_arg(args, int);
            } else {
                prec = 0;
                for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                    prec = prec * 10 + *fmt - '0';
                }
            }
            if (prec < 0) {
                prec = 0;
            }
        }

        // parse long specifiers (current not used)
        //bool long_arg = false;
        if (*fmt == 'l') {
            ++fmt;
            //long_arg = true;
        }

        if (*fmt == '\0') {
            break;
        }

        switch (*fmt) {
            case 'b':
                if (va_arg(args, int)) {
                    chrs += pfenv_print_strn(pfenv, "true", 4, flags, fill, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "false", 5, flags, fill, width);
                }
                break;
            case 'c':
            {
                char str = va_arg(args, int);
                chrs += pfenv_print_strn(pfenv, &str, 1, flags, fill, width);
                break;
            }
            case 's':
            {
                const char *str = va_arg(args, const char*);
                if (str) {
                    if (prec < 0) {
                        prec = strlen(str);
                    }
                    chrs += pfenv_print_strn(pfenv, str, prec, flags, fill, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "(null)", 6, flags, fill, width);
                }
                break;
            }
            case 'u':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 10, 'a', flags, fill, width);
                break;
            case 'd':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 1, 10, 'a', flags, fill, width);
                break;
            case 'x':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'a', flags, fill, width);
                break;
            case 'X':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'A', flags, fill, width);
                break;
            case 'p':
            case 'P': // don't bother to handle upcase for 'P'
                chrs += pfenv_print_int(pfenv, va_arg(args, mp_uint_t), 0, 16, 'a', flags, fill, width);
                break;
#if MICROPY_PY_BUILTINS_FLOAT
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
            {
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
                mp_float_t f = va_arg(args, double);
                chrs += pfenv_print_float(pfenv, f, *fmt, flags, fill, width, prec);
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
                // Currently pfenv_print_float uses snprintf, but snprintf
                // itself may be implemented in terms of pfenv_vprintf() for
                // some ports. So, for extra caution, this case is handled
                // with assert below. Note that currently ports which
                // use MICROPY_FLOAT_IMPL_DOUBLE, don't call pfenv_vprintf()
                // with float format specifier at all.
                // TODO: resolve this completely
                assert(0);
//#error Calling pfenv_print_float with double not supported from within printf
#else
#error Unknown MICROPY FLOAT IMPL
#endif
                break;
            }
#endif
            default:
                pfenv->print_strn(pfenv->data, fmt, 1);
                chrs += 1;
                break;
        }
        ++fmt;
    }
    return chrs;
}

int pfenv_printf(const pfenv_t *pfenv, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = pfenv_vprintf(pfenv, fmt, ap);
    va_end(ap);
    return ret;
}

void printf_wrapper(void *env, const char *fmt, ...) {
    (void)env;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
