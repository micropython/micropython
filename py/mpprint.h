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
#ifndef MICROPY_INCLUDED_PY_MPPRINT_H
#define MICROPY_INCLUDED_PY_MPPRINT_H

#include "py/mpconfig.h"

#define PF_FLAG_LEFT_ADJUST       (0x001)
#define PF_FLAG_SHOW_SIGN         (0x002)
#define PF_FLAG_SPACE_SIGN        (0x004)
#define PF_FLAG_NO_TRAILZ         (0x008)
#define PF_FLAG_SHOW_PREFIX       (0x010)
#define PF_FLAG_SHOW_COMMA        (0x020)
#define PF_FLAG_PAD_AFTER_SIGN    (0x040)
#define PF_FLAG_CENTER_ADJUST     (0x080)
#define PF_FLAG_ADD_PERCENT       (0x100)
#define PF_FLAG_SHOW_OCTAL_LETTER (0x200)

#if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
#define MP_PYTHON_PRINTER &mp_sys_stdout_print
#else
#define MP_PYTHON_PRINTER &mp_plat_print
#endif

typedef void (*mp_print_strn_t)(void *data, const char *str, size_t len);

typedef struct _mp_print_t {
    void *data;
    mp_print_strn_t print_strn;
} mp_print_t;

typedef struct _mp_print_ext_t {
    mp_print_t base;
    const char *item_separator;
    const char *key_separator;
}mp_print_ext_t;

#define MP_PRINT_GET_EXT(print) ((mp_print_ext_t *)print)

// All (non-debug) prints go through one of the two interfaces below.
// 1) Wrapper for platform print function, which wraps MP_PLAT_PRINT_STRN.
extern const mp_print_t mp_plat_print;
#if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
// 2) Wrapper for printing to sys.stdout.
extern const mp_print_t mp_sys_stdout_print;
#endif

int mp_print_str(const mp_print_t *print, const char *str);
int mp_print_strn(const mp_print_t *print, const char *str, size_t len, int flags, char fill, int width);
#if MICROPY_PY_BUILTINS_FLOAT
int mp_print_float(const mp_print_t *print, mp_float_t f, char fmt, int flags, char fill, int width, int prec);
#endif

int mp_printf(const mp_print_t *print, const char *fmt, ...);
#ifdef va_start
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
#endif

// Debug messages during code developing with MP_PRN(level, ...) & MP_PRN_LEVEL.
// An approximate hierarchy of debug levels (MP_PRN_LEVEL) is:
// 0 - SUPPRES all messages. Use it in release version.
// 1 - The most CRITICAL errors, often requiring a system reset, use message with this level if possible raising an exception.
// 2 - ERROR requiring program restart, use message with this level before raising an exception.
// 3 - WARNING, something went wrong, but you can fix it with additional operations in code right now or may ignore it.
// 4 - INFO, it is interesting and useful for understanding a bug.
// 5 - DEBUG, more detaled info, dig deeper.
// 6 - TRACE, show algorithm flow, like enter/exit a function.
// In real you may use own classification of debug levels.
#define MP_PRN(level, ...) \
    do { \
        if (MP_PRN_LEVEL > 0) { \
            if ((0 < level) && (level <= MP_PRN_LEVEL)) { \
                mp_printf(MP_PYTHON_PRINTER, "%d| ", level); \
                mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__); \
                mp_printf(MP_PYTHON_PRINTER, "\n"); \
            } \
        } \
    } while (0);
/*
// How to use:
// Set MP_PRN_LEVEL in developed *.C or *.CPP file, for example

#define MP_PRN_LEVEL 1000 // show all messages
// Add MP_PRN() macro in code, like
void foo() {
    MP_PRN(6, "Enter foo()")
    ...
    int value;
    ...
    // calculate value
    ...
    MP_PRN(4, "See a value=%d", value)
    ...
    MP_PRN(6, "Exit foo()")
}

// See usage in ports/esp32/esp32_pcnt.c and ports/esp32/machine_pwm.c
// It is not a dogma. You may start debugging from level 3.
#define MP_PRN_LEVEL 3
// Then add MP_PRN(3, ...) and when gets too much messages then change some messages to the next level MP_PRN(4, ...), or MP_PRN(2, ...)
// Then you may change MP_PRN_LEVEL to 2(reduce printing), and finally to 0(supress printing).
//
// To switching off MP_PRN() from a compiled binary, use
#ifdef MP_PRN
    #undef MP_PRN
    #define MP_PRN(level, ...)
#endif
*/

#endif // MICROPY_INCLUDED_PY_MPPRINT_H
