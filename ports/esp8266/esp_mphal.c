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
#include "esp_mphal.h"
#include "user_interface.h"
#include "ets_alt_task.h"
#include "py/runtime.h"
#include "extmod/misc.h"
#include "lib/utils/pyexec.h"

STATIC byte input_buf_array[256];
ringbuf_t input_buf = {input_buf_array, sizeof(input_buf_array)};
void mp_hal_debug_tx_strn_cooked(void *env, const char *str, uint32_t len);
const mp_print_t mp_debug_print = {NULL, mp_hal_debug_tx_strn_cooked};

void mp_hal_init(void) {
    //ets_wdt_disable(); // it's a pain while developing
    mp_hal_rtc_init();
    uart_init(UART_BIT_RATE_115200, UART_BIT_RATE_115200);
}

void mp_hal_delay_us(uint32_t us) {
    uint32_t start = system_get_time();
    while (system_get_time() - start < us) {
        ets_event_poll();
    }
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = ringbuf_get(&input_buf);
        if (c != -1) {
            return c;
        }
        #if 0
        // Idles CPU but need more testing before enabling
        if (!ets_loop_iter()) {
            asm("waiti 0");
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

void mp_hal_stdout_tx_str(const char *str) {
    const char *last = str;
    while (*str) {
        uart_tx_one_char(UART0, *str++);
    }
    mp_uos_dupterm_tx_strn(last, str - last);
}

void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    const char *last = str;
    while (len--) {
        uart_tx_one_char(UART0, *str++);
    }
    mp_uos_dupterm_tx_strn(last, str - last);
}

void mp_hal_stdout_tx_strn_cooked(const char *str, uint32_t len) {
    const char *last = str;
    while (len--) {
        if (*str == '\n') {
            if (str > last) {
                mp_uos_dupterm_tx_strn(last, str - last);
            }
            uart_tx_one_char(UART0, '\r');
            uart_tx_one_char(UART0, '\n');
            mp_uos_dupterm_tx_strn("\r\n", 2);
            ++str;
            last = str;
        } else {
            uart_tx_one_char(UART0, *str++);
        }
    }
    if (str > last) {
        mp_uos_dupterm_tx_strn(last, str - last);
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

uint32_t mp_hal_ticks_ms(void) {
    return ((uint64_t)system_time_high_word << 32 | (uint64_t)system_get_time()) / 1000;
}

uint32_t mp_hal_ticks_us(void) {
    return system_get_time();
}

void mp_hal_delay_ms(uint32_t delay) {
    mp_hal_delay_us(delay * 1000);
}

void ets_event_poll(void) {
    ets_loop_iter();
    mp_handle_pending();
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("assert:%s:%d:%s: %s\n", file, line, func, expr);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_AssertionError,
        "C-level assert"));
}

void mp_hal_signal_input(void) {
    #if MICROPY_REPL_EVENT_DRIVEN
    system_os_post(UART_TASK_ID, 0, 0);
    #endif
}

static int call_dupterm_read(void) {
    if (MP_STATE_PORT(term_obj) == NULL) {
        return -1;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t readinto_m[3];
        mp_load_method(MP_STATE_PORT(term_obj), MP_QSTR_readinto, readinto_m);
        readinto_m[2] = MP_STATE_PORT(dupterm_arr_obj);
        mp_obj_t res = mp_call_method_n_kw(1, 0, readinto_m);
        if (res == mp_const_none) {
            nlr_pop();
            return -2;
        }
        if (res == MP_OBJ_NEW_SMALL_INT(0)) {
            mp_uos_deactivate("dupterm: EOF received, deactivating\n", MP_OBJ_NULL);
            nlr_pop();
            return -1;
        }
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(MP_STATE_PORT(dupterm_arr_obj), &bufinfo, MP_BUFFER_READ);
        nlr_pop();
        if (*(byte*)bufinfo.buf == mp_interrupt_char) {
            mp_keyboard_interrupt();
            return -2;
        }
        return *(byte*)bufinfo.buf;
    } else {
        mp_uos_deactivate("dupterm: Exception in read() method, deactivating: ", nlr.ret_val);
    }

    return -1;
}

STATIC void dupterm_task_handler(os_event_t *evt) {
    static byte lock;
    if (lock) {
        return;
    }
    lock = 1;
    while (1) {
        int c = call_dupterm_read();
        if (c < 0) {
            break;
        }
        ringbuf_put(&input_buf, c);
    }
    mp_hal_signal_input();
    lock = 0;
}

STATIC os_event_t dupterm_evt_queue[4];

void dupterm_task_init() {
    system_os_task(dupterm_task_handler, DUPTERM_TASK_ID, dupterm_evt_queue, MP_ARRAY_SIZE(dupterm_evt_queue));
}

void mp_hal_signal_dupterm_input(void) {
    system_os_post(DUPTERM_TASK_ID, 0, 0);
}

// Get pointer to esf_buf bookkeeping structure
void *ets_get_esf_buf_ctlblk(void) {
    // Get literal ptr before start of esf_rx_buf_alloc func
    extern void *esf_rx_buf_alloc();
    return ((void**)esf_rx_buf_alloc)[-1];
}

// Get number of esf_buf free buffers of given type, as encoded by index
// idx 0 corresponds to buf types 1, 2; 1 - 4; 2 - 5; 3 - 7; 4 - 8
// Only following buf types appear to be used:
// 1 - tx buffer, 5 - management frame tx buffer; 8 - rx buffer
int ets_esf_free_bufs(int idx) {
    uint32_t *p = ets_get_esf_buf_ctlblk();
    uint32_t *b = (uint32_t*)p[idx];
    int cnt = 0;
    while (b) {
        b = (uint32_t*)b[0x20 / 4];
        cnt++;
    }
    return cnt;
}

extern int mp_stream_errno;
int *__errno() {
    return &mp_stream_errno;
}
