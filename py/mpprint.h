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
} mp_print_ext_t;

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

#define ANSI_C_MAGENTA      "\x1B[35m"
#define ANSI_C_RED          "\x1B[31m"
#define ANSI_C_YELLOW       "\x1B[33m"
#define ANSI_C_GREEN        "\x1B[32m"
#define ANSI_C_BLUE         "\x1B[34m"
#define ANSI_C_CYAN         "\x1B[36m"
#define ANSI_C_WHITE        "\x1B[37m"
#define ANSI_C_DEFAULT      "\x1B[0m"

#define DO_PRINTF_SUPPRESS 0 // SUPPRESS all messages. Use it in the release version.
#define DO_PRINTF_CRIT     1 // For the most CRITICAL errors, often requiring a system reset. Use a message with this level, if possible, raising an exception.
#define DO_PRINTF_ERROR    2 // ERROR requiring program restart, use message with this level before raising an exception.
#define DO_PRINTF_WARN     3 // WARNING, something went wrong, but you can fix it with additional operations in code right now or may ignore it.
#define DO_PRINTF_INFO     4 // INFO, it is interesting and useful for understanding a bug.
#define DO_PRINTF_DEBUG    5 // DEBUG, more detailed information, dig deeper.
#define DO_PRINTF_TRACE    6 // TRACE, show a flow of the algorithm, like enter/exit a function.

#ifndef DO_PRINTF
#define DO_PRINTF (DO_PRINTF_SUPPRESS)
#endif

#if DO_PRINTF
#define do_printf(...)      mp_printf(&mp_plat_print, __VA_ARGS__)
#define do_print_str(str)   mp_print_str(&mp_plat_print, str)
#define do_print_line()     mp_printf(&mp_plat_print, "\t : FUNC=%s LINE=%d FILE=%s\n", __FUNCTION__, __LINE__, __FILE__)
#else
#define do_printf(...)
#define do_print_str(str)
#define do_print_line()
#endif

// Logging macros.
#if DO_PRINTF >= DO_PRINTF_CRITICAL
#define crit_printf(...)    do_print_str(ANSI_C_MAGENTA); do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define crit_printf(...)
#endif

#if DO_PRINTF >= DO_PRINTF_ERROR
#define error_printf(...)   do_print_str(ANSI_C_RED);     do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define error_printf(...)
#endif

#if DO_PRINTF >= DO_PRINTF_WARN
#define warn_printf(...)    do_print_str(ANSI_C_YELLOW);  do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define warn_printf(...)
#endif

#if DO_PRINTF >= DO_PRINTF_INFO
#define info_printf(...)    do_print_str(ANSI_C_GREEN);   do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define info_printf(...)
#endif

#if DO_PRINTF >= DO_PRINTF_DEBUG
#define debug_printf(...)   do_print_str(ANSI_C_BLUE);    do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define debug_printf(...)
#endif

#if DO_PRINTF >= DO_PRINTF_TRACE
#define trace_printf(...)   do_print_str(ANSI_C_CYAN);    do_printf(__VA_ARGS__); do_print_line(); do_print_str(ANSI_C_DEFAULT);
#else
#define trace_printf(...)
#endif

#if defined(DO_PRINTF) && (DO_PRINTF > 0)

#if defined(MP_DEBUG_PRINT)
#undef MP_DEBUG_PRINT
#endif

#define MP_DEBUG_PRINT(level, ...) \
    do { \
        if ((0 < level) && (level <= DO_PRINTF)) { \
            mp_printf(MP_PYTHON_PRINTER, " %s: ", #level); \
            mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__); \
            mp_printf(MP_PYTHON_PRINTER, "\t : FUNC=%s LINE=%d FILE=%s\n", __FUNCTION__, __LINE__, __FILE__); \
        } \
    } while (0);

#else

#define MP_DEBUG_PRINT(level, ...)

#endif

#endif // MICROPY_INCLUDED_PY_MPPRINT_H
