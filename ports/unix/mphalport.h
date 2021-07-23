/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#include <errno.h>
#include <unistd.h>

#ifndef CHAR_CTRL_C
#define CHAR_CTRL_C (3)
#endif

void mp_hal_set_interrupt_char(char c);

#define mp_hal_stdio_poll unused // this is not implemented, nor needed
void mp_hal_stdio_mode_raw(void);
void mp_hal_stdio_mode_orig(void);

#if MICROPY_PY_BUILTINS_INPUT && MICROPY_USE_READLINE == 0

#include <malloc.h>
#include "py/misc.h"
#include "input.h"
#define mp_hal_readline mp_hal_readline
static inline int mp_hal_readline(vstr_t *vstr, const char *p) {
    char *line = prompt((char *)p);
    vstr_add_str(vstr, line);
    free(line);
    return 0;
}

#elif MICROPY_PY_BUILTINS_INPUT && MICROPY_USE_READLINE == 1

#include "py/misc.h"
#include "shared/readline/readline.h"
// For built-in input() we need to wrap the standard readline() to enable raw mode
#define mp_hal_readline mp_hal_readline
static inline int mp_hal_readline(vstr_t *vstr, const char *p) {
    mp_hal_stdio_mode_raw();
    int ret = readline(vstr, p);
    mp_hal_stdio_mode_orig();
    return ret;
}

#endif

static inline void mp_hal_delay_us(mp_uint_t us) {
    usleep(us);
}
#define mp_hal_ticks_cpu() 0

// This macro is used to implement PEP 475 to retry specified syscalls on EINTR
#define MP_HAL_RETRY_SYSCALL(ret, syscall, raise) { \
        for (;;) { \
            MP_THREAD_GIL_EXIT(); \
            ret = syscall; \
            MP_THREAD_GIL_ENTER(); \
            if (ret == -1) { \
                int err = errno; \
                if (err == EINTR) { \
                    mp_handle_pending(true); \
                    continue; \
                } \
                raise; \
            } \
            break; \
        } \
}

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
      { mp_raise_OSError(error_val); } }

#if MICROPY_PY_BLUETOOTH
enum {
    MP_HAL_MAC_BDADDR,
};

void mp_hal_get_mac(int idx, uint8_t buf[6]);
#endif
