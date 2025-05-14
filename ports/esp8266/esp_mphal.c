/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "ets_sys.h"
#include "etshal.h"
#include "uart.h"
#include "user_interface.h"
#include "ets_alt_task.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "extmod/misc.h"
#include "shared/runtime/pyexec.h"

static byte stdin_ringbuf_array[256];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};
void mp_hal_debug_tx_strn_cooked(void *env, const char *str, uint32_t len);
const mp_print_t mp_debug_print = {NULL, mp_hal_debug_tx_strn_cooked};

int uart_attached_to_dupterm;

void mp_hal_init(void) {
    // ets_wdt_disable(); // it's a pain while developing
    mp_hal_rtc_init();
    uart_init(UART_BIT_RATE_115200, UART_BIT_RATE_115200);
    uart_attached_to_dupterm = 0;
}

void MP_FASTCODE(mp_hal_delay_us)(uint32_t us) {
    uint32_t start = system_get_time();
    while (system_get_time() - start < us) {
        mp_event_handle_nowait();
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= mp_os_dupterm_poll(poll_flags);
    }
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #if 0
        // Idles CPU but need more testing before enabling
        if (!ets_loop_iter()) {
            asm ("waiti 0");
        }
        #else
        mp_hal_delay_us(1);
        #endif
    }
}

#if 0
void mp_hal_debug_str(const char *str) {
    while (*str) {
        uart_tx_one_char(UART0, *str++);
    }
    uart_flush(UART0);
}
#endif

mp_uint_t mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res < 0) {
        // no outputs, nothing was written
        return 0;
    } else {
        return dupterm_res;
    }
}

void mp_hal_debug_tx_strn_cooked(void *env, const char *str, uint32_t len) {
    (void)env;
    while (len--) {
        if (*str == '\n') {
            uart_tx_one_char(UART0, '\r');
        }
        uart_tx_one_char(UART0, *str++);
    }
}

uint32_t MP_FASTCODE(mp_hal_ticks_ms)(void) {
    // Compute milliseconds from 64-bit microsecond counter
    system_time_update();
    return ((uint64_t)system_time_high_word << 32 | (uint64_t)system_time_low_word) / 1000;
}

void MP_FASTCODE(mp_hal_delay_ms)(uint32_t delay) {
    mp_hal_delay_us(delay * 1000);
}

uint64_t mp_hal_time_ns(void) {
    return pyb_rtc_get_us_since_epoch() * 1000ULL;
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("assert:%s:%d:%s: %s\n", file, line, func, expr);
    mp_raise_msg(&mp_type_AssertionError, MP_ERROR_TEXT("C-level assert"));
}

// May be called by uart0_rx_intr_handler.
void MP_FASTCODE(mp_hal_signal_input)(void) {
    #if MICROPY_REPL_EVENT_DRIVEN
    system_os_post(UART_TASK_ID, 0, 0);
    #endif
}

// this bit is unused in the Xtensa PS register
#define ETS_LOOP_ITER_BIT (12)

uint32_t esp_disable_irq(void) {
    uint32_t state = disable_irq();
    state = (state & ~(1 << ETS_LOOP_ITER_BIT)) | (ets_loop_iter_disable << ETS_LOOP_ITER_BIT);
    ets_loop_iter_disable = 1;
    return state;
}

void esp_enable_irq(uint32_t state) {
    ets_loop_iter_disable = (state >> ETS_LOOP_ITER_BIT) & 1;
    enable_irq(state & ~(1 << ETS_LOOP_ITER_BIT));
}

// Get pointer to esf_buf bookkeeping structure
void *ets_get_esf_buf_ctlblk(void) {
    // Get literal ptr before start of esf_rx_buf_alloc func
    extern void *esf_rx_buf_alloc();
    return ((void **)esf_rx_buf_alloc)[-1];
}

// Get number of esf_buf free buffers of given type, as encoded by index
// idx 0 corresponds to buf types 1, 2; 1 - 4; 2 - 5; 3 - 7; 4 - 8
// Only following buf types appear to be used:
// 1 - tx buffer, 5 - management frame tx buffer; 8 - rx buffer
int ets_esf_free_bufs(int idx) {
    uint32_t *p = ets_get_esf_buf_ctlblk();
    uint32_t *b = (uint32_t *)p[idx];
    int cnt = 0;
    while (b) {
        b = (uint32_t *)b[0x20 / 4];
        cnt++;
    }
    return cnt;
}
