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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "extmod/misc.h"

#ifdef __linux__
#include <sys/eventfd.h>
// An eventfd is read and written as a 64-bit counter.
typedef uint64_t wake_event_token_t;
#else
// The self-pipe carries one byte per wakeup.
typedef uint8_t wake_event_token_t;
#endif

// The wake event: a Linux eventfd, or a self-pipe elsewhere.  Reading
// wake_event_fd drains it, writing wake_event_wr_fd raises it; on Linux both
// are the same descriptor, and both are negative when not initialised.
static int wake_event_fd = -1;
static int wake_event_wr_fd = -1;

#ifndef __linux__
static int set_cloexec_nonblock(int fd) {
    // pipe2() would do this atomically at creation but is absent on macOS.
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
        return -1;
    }
    return fcntl(fd, F_SETFD, FD_CLOEXEC);
}
#endif

void mp_hal_wake_event_init(void) {
    #ifdef __linux__
    wake_event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    wake_event_wr_fd = wake_event_fd;
    #else
    int fds[2];
    if (pipe(fds) == 0) {
        if (set_cloexec_nonblock(fds[0]) == 0 && set_cloexec_nonblock(fds[1]) == 0) {
            wake_event_fd = fds[0];
            wake_event_wr_fd = fds[1];
        } else {
            close(fds[0]);
            close(fds[1]);
        }
    }
    #endif
    if (wake_event_fd < 0) {
        fprintf(stderr, "FATAL: cannot create wake event: %s\n", strerror(errno));
        exit(1);
    }
    if (wake_event_fd >= FD_SETSIZE) {
        // Waiting on it uses select(), and FD_SET() is undefined from here up.
        fprintf(stderr, "FATAL: wake event fd %d exceeds FD_SETSIZE\n", wake_event_fd);
        exit(1);
    }
}

void mp_hal_wake_event_deinit(void) {
    // Cleared before closing, so a concurrent raise is dropped rather than
    // written to a reused descriptor.
    int rd = wake_event_fd;
    int wr = wake_event_wr_fd;
    wake_event_fd = -1;
    wake_event_wr_fd = -1;
    if (wr != rd) {
        close(wr);
    }
    close(rd);
}

static void wake_event_drain(int fd) {
    // Empty it fully or the next wait returns at once: an eventfd reads its
    // counter in one go, a self-pipe holds a byte per raise.
    char buf[64];
    while (read(fd, buf, sizeof(buf)) > 0) {
    }
}

void mp_hal_signal_event(void) {
    int fd = wake_event_wr_fd;
    if (fd < 0) {
        return;
    }
    // Reachable from a signal handler, so no allocation, and errno is
    // preserved.  A failed write means the event is already raised.
    int errno_save = errno;
    wake_event_token_t token = 1;
    ssize_t ret = write(fd, &token, sizeof(token));
    (void)ret;
    errno = errno_save;
}

int mp_hal_wake_event_wait_tv(struct timeval *tv) {
    // Only the thread that runs scheduled callbacks may consume the event: one
    // taken by another thread is one the thread that can act on it never sees.
    // Other threads just wait out the timeout.
    int fd = wake_event_fd;
    if (fd < 0 || !mp_sched_thread_can_run_callbacks()) {
        return select(0, NULL, NULL, NULL, tv);
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    int ret = select(fd + 1, &rfds, NULL, NULL, tv);
    if (ret > 0) {
        wake_event_drain(fd);
        errno = EINTR;
        return -1;
    }
    return ret;
}

void mp_hal_wake_event_wait_ms(mp_uint_t timeout_ms) {
    if (timeout_ms == MP_HAL_WAKE_EVENT_FOREVER) {
        mp_hal_wake_event_wait_tv(NULL);
        return;
    }
    struct timeval tv = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    mp_hal_wake_event_wait_tv(&tv);
}

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

void mp_hal_set_interrupt_char(char c) {
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
    tcsetattr(0, TCSANOW, &termios);
}

void mp_hal_stdio_mode_orig(void) {
    // restore terminal settings
    tcsetattr(0, TCSANOW, &orig_termios);
}

#endif

#if MICROPY_PY_OS_DUPTERM
static int call_dupterm_read(size_t idx) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t read_m[3];
        mp_load_method(MP_STATE_VM(dupterm_objs[idx]), MP_QSTR_read, read_m);
        read_m[2] = MP_OBJ_NEW_SMALL_INT(1);
        mp_obj_t res = mp_call_method_n_kw(1, 0, read_m);
        if (res == mp_const_none) {
            return -2;
        }
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(res, &bufinfo, MP_BUFFER_READ);
        if (bufinfo.len == 0) {
            mp_printf(&mp_plat_print, "dupterm: EOF received, deactivating\n");
            MP_STATE_VM(dupterm_objs[idx]) = MP_OBJ_NULL;
            return -1;
        }
        nlr_pop();
        return *(byte *)bufinfo.buf;
    } else {
        // Temporarily disable dupterm to avoid infinite recursion
        mp_obj_t save_term = MP_STATE_VM(dupterm_objs[idx]);
        MP_STATE_VM(dupterm_objs[idx]) = NULL;
        mp_printf(&mp_plat_print, "dupterm: ");
        mp_obj_print_exception(&mp_plat_print, nlr.ret_val);
        MP_STATE_VM(dupterm_objs[idx]) = save_term;
    }

    return -1;
}
#endif

int mp_hal_stdin_rx_chr(void) {
    #if MICROPY_PY_OS_DUPTERM
    // TODO only support dupterm one slot at the moment
    if (MP_STATE_VM(dupterm_objs[0]) != MP_OBJ_NULL) {
        int c;
        do {
            c = call_dupterm_read(0);
        } while (c == -2);
        if (c == -1) {
            goto main_term;
        }
        if (c == '\n') {
            c = '\r';
        }
        return c;
    }
main_term:;
    #endif

    unsigned char c;
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, read(STDIN_FILENO, &c, 1), {});
    if (ret == 0) {
        c = 4; // EOF, ctrl-D
    } else if (c == '\n') {
        c = '\r';
    }
    return c;
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, write(STDOUT_FILENO, str, len), {});
    mp_uint_t written = ret < 0 ? 0 : ret;
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        written = MIN((mp_uint_t)dupterm_res, written);
    }
    return written;
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
        mp_uint_t elapsed = 0;
        // The wait can return early, so loop until the time is up.
        while (elapsed < ms) {
            mp_event_wait_ms(ms - elapsed);
            elapsed = mp_hal_ticks_ms() - start;
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
