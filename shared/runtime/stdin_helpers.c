/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include "py/runtime.h"
#include "py/ringbuf.h"
#include "py/stream.h"

#if MICROPY_KBD_EXCEPTION
#include "interrupt_char.h"
#endif

#ifndef MICROPY_STDIN_BUFF_SIZE
#define MICROPY_STDIN_BUFF_SIZE 260
#endif

STATIC uint8_t stdin_ringbuf_array[MICROPY_STDIN_BUFF_SIZE];
static ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

uintptr_t mp_hal_stdin_rx_buff_poll(uintptr_t poll_flags) {
    if (poll_flags & MP_STREAM_POLL_RD) {
        if (stdin_ringbuf.iget != stdin_ringbuf.iput) {
            return MP_STREAM_POLL_RD;
        }
    }
    return 0;
}

int mp_hal_stdin_rx_buff_get(void) {
    int c = ringbuf_get(&stdin_ringbuf);
    #ifdef MICROPY_EVENT_POLL_HOOK
    if (c != -1) {
        return c;
    }
    MICROPY_EVENT_POLL_HOOK
    #endif
    return c;
}

int mp_hal_stdin_rx_buff_put(char c) {
    #if MICROPY_KBD_EXCEPTION
    if (c == mp_interrupt_char) {
        mp_sched_keyboard_interrupt();
        return 0;
    }
    #endif
    return ringbuf_put(&stdin_ringbuf, c);
}

size_t mp_hal_stdin_rx_buff_free() {
    return ringbuf_free(&stdin_ringbuf);
}
