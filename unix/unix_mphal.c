/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "py/mpstate.h"
#include "lib/mp-readline/readline.h"
#include "unix_mphal.h"

static struct termios orig_termios;
static struct termios cur_termios;
static char interrupt_char;

#ifndef _WIN32
#include <signal.h>

static struct sigaction sa;

STATIC void sighandler(int signum) {
    if (signum == SIGINT && interrupt_char == CHAR_CTRL_C) {
        if (MP_STATE_VM(mp_pending_exception) == MP_STATE_VM(keyboard_interrupt_obj)) {
            mp_hal_deinit();
            exit(1);
        }
        mp_obj_exception_clear_traceback(MP_STATE_VM(keyboard_interrupt_obj));
        MP_STATE_VM(mp_pending_exception) = MP_STATE_VM(keyboard_interrupt_obj);
        // disable our handler so next we really die

        /*
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, NULL);
        */
    }
}
#endif

void mp_hal_init(void) {
    tcgetattr(0, &orig_termios);
    cur_termios = orig_termios;
    cur_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    cur_termios.c_cflag = (cur_termios.c_cflag & ~(CSIZE | PARENB)) | CS8;
    cur_termios.c_lflag = 0;
    cur_termios.c_cc[VMIN] = 1;
    cur_termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &cur_termios);

    interrupt_char = -1;

    #ifndef _WIN32
    // enable signal handler
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = SIG_DFL;
    #endif
}

void mp_hal_deinit(void) {
    #ifndef _WIN32
    // disable signal handler
    sigaction(SIGINT, &sa, NULL);
    #endif
    tcsetattr(0, TCSAFLUSH, &orig_termios);
}

void mp_hal_set_interrupt_char(char c) {
    interrupt_char = c;
    if (c == -1) {
        cur_termios.c_lflag = 0;
        tcsetattr(0, TCSAFLUSH, &cur_termios);
    } else {
        cur_termios.c_lflag = ISIG;
        tcsetattr(0, TCSAFLUSH, &cur_termios);
    }
}

int mp_hal_stdin_rx_chr(void) {
    unsigned char c;
    int ret = read(0, &c, 1);
    if (ret == 0) {
        c = 4; // EOF, ctrl-D
    } else if (c == '\n') {
        c = '\r';
    }
    return c;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    write(1, str, len);
}

// cooked is same as uncooked because they terminal does some postprocessing
void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    mp_hal_stdout_tx_strn(str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
