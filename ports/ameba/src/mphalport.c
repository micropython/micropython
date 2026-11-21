/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Realtek Corporation
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

#include "ameba.h"
#include "os_wrapper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mpconfigport.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

static uint8_t uart_ringbuf_array[256];
// SPSC: log_uart_irq (ISR) is the sole producer; mp_hal_stdin_rx_chr (task) is the sole consumer.
ringbuf_t stdin_ringbuf = {uart_ringbuf_array, sizeof(uart_ringbuf_array), 0, 0};

// Counts bytes dropped when the RX ring buffer is full.
static volatile uint32_t uart_rx_overflow;

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_os_dupterm_poll(poll_flags);
    #endif
    // Check ringbuffer directly for uart and usj.
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    // DiagPrintfNano("%s %x\r\n", __FUNCTION__, __builtin_return_address(0));
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

extern void LOGUART_PutChar_RAM(u8 c);
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    mp_uint_t nChars = 0;
    // DiagPrintfNano("%s %x\r\n", __FUNCTION__, __builtin_return_address(0));
    for (; len > 0; --len) {
        LOGUART_PutChar_RAM(*str++);
        ++nChars;
    }
    return nChars;
}


mp_uint_t mp_hal_ticks_ms(void) {
    return rtos_time_get_current_system_time_ms_64bit();
}

mp_uint_t mp_hal_ticks_us(void) {
    return rtos_time_get_current_system_time_us();
}
mp_uint_t mp_hal_ticks_cpu(void) {
    return rtos_time_get_current_system_time_ns();
}

uint64_t mp_hal_time_ns(void) {
    return rtos_time_get_current_system_time_ns();
}

void mp_hal_delay_us(mp_uint_t us) {
    // these constants are tested for a 240MHz clock
    const uint32_t this_overhead = 5;
    const uint32_t pend_overhead = 150;

    // return if requested delay is less than calling overhead
    if (us < this_overhead) {
        return;
    }
    us -= this_overhead;

    uint64_t t0 = mp_hal_ticks_us();
    for (;;) {
        uint64_t dt = mp_hal_ticks_us() - t0;
        if (dt >= us) {
            return;
        }
        if (dt + pend_overhead < us) {
            // we have enough time to service pending events
            // (don't use MICROPY_EVENT_POLL_HOOK because it also yields)
            mp_handle_pending(true);
        }
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint64_t us = (uint64_t)ms * 1000ULL;
    uint64_t dt;
    uint64_t t0 = mp_hal_ticks_us();
    for (;;) {
        mp_handle_pending(true);
        // MICROPY_PY_SOCKET_EVENTS_HANDLER
        MP_THREAD_GIL_EXIT();
        uint64_t t1 = mp_hal_ticks_us();
        dt = t1 - t0;
        if (dt + portTICK_PERIOD_MS * 1000ULL >= us) {
            // doing a vTaskDelay would take us beyond requested delay time
            taskYIELD();
            MP_THREAD_GIL_ENTER();
            t1 = mp_hal_ticks_us();
            dt = t1 - t0;
            break;
        } else {
            ulTaskNotifyTake(pdFALSE, 1);
            MP_THREAD_GIL_ENTER();
        }
    }
    if (dt < us) {
        // do the remaining delay accurately
        mp_hal_delay_us(us - dt);
    }
}

#if !MICROPY_KBD_EXCEPTION
void mp_hal_set_interrupt_char(int c) {

}
#endif
u32 log_uart_irq(void *Data) {
    // To avoid gcc warnings
    (void)Data;
    u32 loguart_lsr = LOGUART_GetStatus(LOGUART_DEV);

    if (!(loguart_lsr & LOGUART_BIT_DRDY)) {
        return 0;
    }
    bool PullMode = FALSE;
    u32 LogUartIrqEn = LOGUART_GetIMR();
    LOGUART_SetIMR(0);

    int len = LOGUART_GetRxCount();
    for (int i = 0; i < len; i++) {
        char cdata = LOGUART_GetChar(PullMode);
        if (cdata == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            if (ringbuf_put(&stdin_ringbuf, cdata) < 0) {
                ++uart_rx_overflow;
            }
        }
    }

    LOGUART_SetIMR(LogUartIrqEn);

    return 0;
}

void rtk_loguart_init(void) {
    #if defined(MICROPY_HW_LOGUART_BAUDRATE)
    // ROM brings LOGUART up at 1500000. Drain any boot output still queued at the
    // ROM baud before switching, so the baud change doesn't corrupt in-flight bytes.
    LOGUART_WaitTxComplete();
    LOGUART_SetBaud(LOGUART_DEV, MICROPY_HW_LOGUART_BAUDRATE);
    #endif

    // Register Log Uart Callback function
    InterruptRegister((IRQ_FUN)log_uart_irq, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
    InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
    LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM4, ENABLE);
    LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM0, DISABLE);
}

void mp_hal_get_random(size_t n, void *buf) {
    TRNG_get_random_bytes(buf, (u32)n);
}
