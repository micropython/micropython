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

#include "py/mpconfig.h"

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "py/obj.h"
#include "py/mphal.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include "py/formatfloat.h"
#endif

#if MICROPY_DEBUG_PRINTERS
int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(MICROPY_DEBUG_PRINTER, fmt, ap);
    va_end(ap);
    return ret;
}
#endif

#if MICROPY_USE_INTERNAL_PRINTF

#undef putchar  // Some stdlibs have a #define for putchar
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
int putchar(int c);
int puts(const char *s);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int snprintf(char *str, size_t size, const char *fmt, ...);

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(&mp_plat_print, fmt, ap);
    va_end(ap);
    return ret;
}

int vprintf(const char *fmt, va_list ap) {
    return mp_vprintf(&mp_plat_print, fmt, ap);
}

// need this because gcc optimises printf("%c", c) -> putchar(c), and printf("a") -> putchar('a')
int putchar(int c) {
    char chr = c;
    mp_hal_stdout_tx_strn_cooked(&chr, 1);
    return chr;
}

// need this because gcc optimises printf("string\n") -> puts("string")
int puts(const char *s) {
    mp_hal_stdout_tx_strn_cooked(s, strlen(s));
    char chr = '\n';
    mp_hal_stdout_tx_strn_cooked(&chr, 1);
    return 1;
}

typedef struct _strn_print_env_t {
    char *cur;
    size_t remain;
} strn_print_env_t;

STATIC void strn_print_strn(void *data, const char *str, size_t len) {
    strn_print_env_t *strn_print_env = data;
    if (len > strn_print_env->remain) {
        len = strn_print_env->remain;
    }
    memcpy(strn_print_env->cur, str, len);
    strn_print_env->cur += len;
    strn_print_env->remain -= len;
}

#if defined(__GNUC__) && !defined(__clang__)
// uClibc requires this alias to be defined, or there may be link errors
// when linkings against it statically.
int __GI_vsnprintf(char *str, size_t size, const char *fmt, va_list ap) __attribute__((weak, alias ("vsnprintf")));
#endif

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    strn_print_env_t strn_print_env = {str, size};
    mp_print_t print = {&strn_print_env, strn_print_strn};
    int len = mp_vprintf(&print, fmt, ap);
    // add terminating null byte
    if (size > 0) {
        if (strn_print_env.remain == 0) {
            strn_print_env.cur[-1] = 0;
        } else {
            strn_print_env.cur[0] = 0;
        }
    }
    return len;
}

int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
}

#endif //MICROPY_USE_INTERNAL_PRINTF
