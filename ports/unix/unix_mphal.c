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
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>

#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "extmod/misc.h"

#if MICROPY_ENABLE_SCHEDULER && !defined(_WIN32)

// Signal used to wake blocking waits when a callback is scheduled. Use a
// real-time signal if available, above those used by mpthreadport.c.
// Fall back to SIGURG, which is ignored by default and rarely used.
#ifdef SIGRTMIN
#define MP_SCHED_SIGNAL (SIGRTMIN + 7)
#else
#define MP_SCHED_SIGNAL (SIGURG)
#endif

// Longest single event wait, so an indefinite wait degrades to bounded
// latency in case a wakeup is ever missed.
#define MP_SCHED_SLEEP_MAX_MS (1000)

static bool sched_signal_installed;

static void sched_sighandler(int signum) {
    (void)signum;
}

void mp_unix_init_sched_signal(void) {
    struct sigaction sa;
    sa.sa_flags = 0; // No SA_RESTART: pselect() returns EINTR.
    sa.sa_handler = sched_sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(MP_SCHED_SIGNAL, &sa, NULL) != 0) {
        // Signal unavailable on this platform: waits fall back to plain
        // timeouts and mp_hal_signal_event() does nothing.
        return;
    }
    // Mask delivery of the signal in every thread (children inherit this
    // mask). Raising it then never interrupts any thread; it just stays
    // queued on the process until a waiter accepts it by unmasking it
    // inside pselect(), in mp_unix_sched_select().
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, MP_SCHED_SIGNAL);
    #if MICROPY_PY_THREAD
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    #else
    sigprocmask(SIG_BLOCK, &set, NULL);
    #endif
    sched_signal_installed = true;
}

void mp_unix_deinit_sched_signal(void) {
    if (!sched_signal_installed) {
        return;
    }
    sched_signal_installed = false;
    // SIG_IGN discards any still-pending signal; SIG_DFL would terminate
    // the process on a late delivery (real-time signals default to Term).
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sigaction(MP_SCHED_SIGNAL, &sa, NULL);
}

void mp_hal_signal_event(void) {
    if (sched_signal_installed) {
        // Async-signal-safe. Delivery is masked in every thread, so this
        // only marks the signal pending for a waiter; no thread is
        // interrupted.
        kill(getpid(), MP_SCHED_SIGNAL);
    }
}

// Sleep for the given duration or until woken by mp_hal_signal_event().
// pselect() unmasks the wakeup signal only while waiting, so a wakeup
// raised at any earlier point is still pending and aborts the wait
// immediately. Returns 0 on timeout, -1 with errno EINTR on wakeup.
int mp_unix_sched_select(struct timeval *tv) {
    if (!sched_signal_installed) {
        return select(0, NULL, NULL, NULL, tv);
    }
    struct timespec ts;
    ts.tv_sec = tv->tv_sec;
    ts.tv_nsec = tv->tv_usec * 1000;
    sigset_t waitmask;
    #if MICROPY_PY_THREAD
    pthread_sigmask(SIG_BLOCK, NULL, &waitmask);
    #else
    sigprocmask(SIG_BLOCK, NULL, &waitmask);
    #endif
    sigdelset(&waitmask, MP_SCHED_SIGNAL);
    return pselect(0, NULL, NULL, NULL, &ts, &waitmask);
}

// Wait for an event (scheduled callback) or timeout, for
// MICROPY_INTERNAL_WFE. May wake spuriously, at most once per stale
// pending signal; callers loop and re-check for events.
void mp_unix_sched_sleep(uint32_t timeout_ms) {
    if (MP_STATE_VM(sched_state) == MP_SCHED_PENDING) {
        return;
    }
    if (timeout_ms > MP_SCHED_SLEEP_MAX_MS) {
        timeout_ms = MP_SCHED_SLEEP_MAX_MS;
    }
    struct timeval tv = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    mp_unix_sched_select(&tv);
}
#endif

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 25)
#include <sys/random.h>
#define _HAVE_GETRANDOM
#endif
#endif

#ifndef _WIN32
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
        #if MICROPY_ENABLE_SCHEDULER
        // Keep the scheduler wakeup signal masked: it must stay pending
        // until accepted by a waiter (see mp_unix_init_sched_signal). This
        // also restores the steady-state mask when the longjmp cuts short
        // a pselect() that had it temporarily unmasked.
        sigaddset(&mask, MP_SCHED_SIGNAL);
        #endif
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
