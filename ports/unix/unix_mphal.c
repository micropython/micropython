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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <poll.h>

#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/objtype.h"
#include "extmod/misc.h"

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 25)
#include <sys/random.h>
#define _HAVE_GETRANDOM
#endif
#endif

#ifndef _WIN32
#include <signal.h>

static void sighandler(int signum) {
    if (signum == SIGINT) {
        #if MICROPY_ASYNC_KBD_INTR
        #if MICROPY_PY_THREAD_GIL
        // Since signals can occur at any time, we may not be holding the GIL when
        // this callback is called, so it is not safe to raise an exception here
        #error "MICROPY_ASYNC_KBD_INTR and MICROPY_PY_THREAD_GIL are not compatible"
        #endif
        mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)));
        sigset_t mask;
        sigemptyset(&mask);
        // On entry to handler, its signal is blocked, and unblocked on
        // normal exit. As we instead perform longjmp, unblock it manually.
        sigprocmask(SIG_SETMASK, &mask, NULL);
        nlr_raise(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)));
        #else
        if (MP_STATE_MAIN_THREAD(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception))) {
            // this is the second time we are called, so die straight away
            exit(1);
        }
        mp_sched_keyboard_interrupt();
        #endif
    }
}
#endif

#if MICROPY_KBD_EXCEPTION
int mp_interrupt_char = -1;
#endif

void mp_hal_set_interrupt_char(char c) {
    #if MICROPY_KBD_EXCEPTION
    mp_interrupt_char = c;
    #endif
    // configure terminal settings to (not) let ctrl-C through
    if (c == CHAR_CTRL_C) {
        #ifndef _WIN32
        // enable signal handler
        struct sigaction sa;
        sa.sa_flags = 0;
        sa.sa_handler = sighandler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, NULL);
        #endif
    } else {
        #ifndef _WIN32
        // disable signal handler
        struct sigaction sa;
        sa.sa_flags = 0;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, NULL);
        #endif
    }
}

#if MICROPY_USE_READLINE == 1

#include <termios.h>

static struct termios orig_termios;

void mp_hal_stdio_mode_raw(void) {
    // save and set terminal settings
    tcgetattr(0, &orig_termios);
    static struct termios termios;
    termios = orig_termios;
    termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    termios.c_cflag = (termios.c_cflag & ~(CSIZE | PARENB)) | CS8;
    termios.c_lflag = 0;
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &termios);
}

void mp_hal_stdio_mode_orig(void) {
    // restore terminal settings
    tcsetattr(0, TCSAFLUSH, &orig_termios);
}

#endif

#if MICROPY_PY_OS_DUPTERM
// Builtin stream object that wraps STDIN/STDOUT file descriptors.
// This is registered as dupterm slot 0 so that os.dupterm() can manage
// the default console alongside user-provided streams.

typedef struct _mp_unix_stdio_obj_t {
    mp_obj_base_t base;
} mp_unix_stdio_obj_t;

static mp_uint_t mp_unix_stdio_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    (void)self_in;
    // Use non-blocking read so dupterm can poll multiple slots.
    struct pollfd pfd = { .fd = STDIN_FILENO, .events = POLLIN };
    if (poll(&pfd, 1, 0) <= 0 || !(pfd.revents & POLLIN)) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, read(STDIN_FILENO, buf, size), {
        *errcode = err;
        return MP_STREAM_ERROR;
    });
    if (ret == 0) {
        // EOF
        *errcode = 0;
        return 0;
    }
    return ret;
}

static mp_uint_t mp_unix_stdio_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    (void)self_in;
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, write(STDOUT_FILENO, buf, size), {
        *errcode = err;
        return MP_STREAM_ERROR;
    });
    return ret;
}

static mp_uint_t mp_unix_stdio_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)self_in;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        uintptr_t ret = 0;
        if (flags & MP_STREAM_POLL_RD) {
            struct pollfd pfd = { .fd = STDIN_FILENO, .events = POLLIN };
            if (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN)) {
                ret |= MP_STREAM_POLL_RD;
            }
        }
        if (flags & MP_STREAM_POLL_WR) {
            ret |= MP_STREAM_POLL_WR;
        }
        return ret;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

static const mp_stream_p_t mp_unix_stdio_stream_p = {
    .read = mp_unix_stdio_read,
    .write = mp_unix_stdio_write,
    .ioctl = mp_unix_stdio_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_unix_stdio_type,
    MP_QSTR_stdio,
    MP_TYPE_FLAG_NONE,
    protocol, &mp_unix_stdio_stream_p
    );

mp_unix_stdio_obj_t mp_unix_stdio_obj = {{&mp_unix_stdio_type}};

bool mp_os_dupterm_is_builtin_stream(mp_const_obj_t stream) {
    return mp_obj_get_type(stream) == &mp_unix_stdio_type;
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return mp_os_dupterm_poll(poll_flags);
}
#endif // MICROPY_PY_OS_DUPTERM

int mp_hal_stdin_rx_chr(void) {
    #if MICROPY_PY_OS_DUPTERM
    for (;;) {
        int c = mp_os_dupterm_rx_chr();
        if (c >= 0) {
            if (c == '\n') {
                c = '\r';
            }
            return c;
        }
        mp_event_wait_indefinite();
    }
    #else
    unsigned char c;
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, read(STDIN_FILENO, &c, 1), {});
    if (ret == 0) {
        c = 4; // EOF, ctrl-D
    } else if (c == '\n') {
        c = '\r';
    }
    return c;
    #endif
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    #if MICROPY_PY_OS_DUPTERM
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        return dupterm_res;
    }
    // No dupterm streams active, write directly.
    #endif
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, write(STDOUT_FILENO, str, len), {});
    return ret < 0 ? 0 : ret;
}

// cooked is same as uncooked because the terminal does some postprocessing
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    mp_hal_stdout_tx_strn(str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

#ifndef mp_hal_ticks_ms
mp_uint_t mp_hal_ticks_ms(void) {
    #if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0) && defined(_POSIX_MONOTONIC_CLOCK)
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
    #else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    #endif
}
#endif

#ifndef mp_hal_ticks_us
mp_uint_t mp_hal_ticks_us(void) {
    #if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0) && defined(_POSIX_MONOTONIC_CLOCK)
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
    #else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
    #endif
}
#endif

#ifndef mp_hal_time_ns
uint64_t mp_hal_time_ns(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000000ULL + (uint64_t)tv.tv_usec * 1000ULL;
}
#endif

#ifndef mp_hal_delay_ms
void mp_hal_delay_ms(mp_uint_t ms) {
    if (ms) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (mp_hal_ticks_ms() - start < ms) {
            mp_event_wait_ms(1);
        }
    } else {
        mp_handle_pending(true);
    }
}
#endif

void mp_hal_get_random(size_t n, uint8_t *buf) {
    #ifdef _HAVE_GETRANDOM
    RAISE_ERRNO(getrandom(buf, n, 0), errno);
    #else
    int fd = open("/dev/random", O_RDONLY);
    RAISE_ERRNO(fd, errno);
    RAISE_ERRNO(read(fd, buf, n), errno);
    close(fd);
    #endif
}
