/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "shared/runtime/interrupt_char.h"
#include "extmod/misc.h"
#include "samd_soc.h"
#include "tusb.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 128
#endif

extern volatile uint32_t ticks_us64_upper;

STATIC uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0 };

uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll

void poll_cdc_interfaces(void) {
    // any CDC interfaces left to poll?
    if (cdc_itf_pending && ringbuf_free(&stdin_ringbuf)) {
        for (uint8_t itf = 0; itf < 8; ++itf) {
            if (cdc_itf_pending & (1 << itf)) {
                tud_cdc_rx_cb(itf);
                if (!cdc_itf_pending) {
                    break;
                }
            }
        }
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            #if MICROPY_KBD_EXCEPTION
            if (data_char == mp_interrupt_char) {
                // Clear the ring buffer
                stdin_ringbuf.iget = stdin_ringbuf.iput = 0;
                // and stop
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
            #else
            ringbuf_put(&stdin_ringbuf, data_char);
            #endif
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
}

void mp_hal_set_pin_mux(mp_hal_pin_obj_t pin, uint8_t mux) {
    int pin_grp = pin / 32;
    int port_grp = (pin % 32) / 2;
    PORT->Group[pin_grp].PINCFG[pin % 32].bit.PMUXEN = 1; // Enable Mux
    if (pin & 1) {
        PORT->Group[pin_grp].PMUX[port_grp].bit.PMUXO = mux;
    } else {
        PORT->Group[pin_grp].PMUX[port_grp].bit.PMUXE = mux;
    }
}

void mp_hal_clr_pin_mux(mp_hal_pin_obj_t pin) {
    int pin_grp = pin / 32;
    PORT->Group[pin_grp].PINCFG[pin % 32].bit.PMUXEN = 0; // Disable Mux
}

void mp_hal_delay_ms(mp_uint_t ms) {
    if (ms > 10) {
        uint32_t t0 = systick_ms;
        while (systick_ms - t0 < ms) {
            MICROPY_EVENT_POLL_HOOK
        }
    } else {
        mp_hal_delay_us(ms * 1000);
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    if (us > 0) {
        #if defined(MCU_SAMD21)
        uint32_t start = mp_hal_ticks_us();
        while ((mp_hal_ticks_us() - start) < us) {
        }
        #else
        uint64_t stop = mp_hal_ticks_us_64() + us;
        while (mp_hal_ticks_us_64() < stop) {
        }
        #endif
    }
}

uint64_t mp_hal_ticks_us_64(void) {
    uint32_t us64_upper = ticks_us64_upper;
    uint32_t us64_lower;
    uint8_t intflag;
    __disable_irq();
    #if defined(MCU_SAMD21)
    us64_lower = REG_TC4_COUNT32_COUNT;
    intflag = TC4->COUNT32.INTFLAG.reg;
    #elif defined(MCU_SAMD51)
    TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
    while (TC0->COUNT32.CTRLBSET.reg != 0) {
    }
    us64_lower = REG_TC0_COUNT32_COUNT;
    intflag = TC0->COUNT32.INTFLAG.reg;
    #endif
    __enable_irq();
    if ((intflag & TC_INTFLAG_OVF) && us64_lower < 0x10000000) {
        // The timer counter overflowed before reading it but the IRQ handler
        // has not yet been called, so perform the IRQ arithmetic now.
        us64_upper++;
    }
    #if defined(MCU_SAMD21)
    return ((uint64_t)us64_upper << 32) | us64_lower;
    #elif defined(MCU_SAMD51)
    return ((uint64_t)us64_upper << 28) | (us64_lower >> 4);
    #endif

}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;

    poll_cdc_interfaces();
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }

    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_uos_dupterm_poll(poll_flags);
    #endif
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {

        poll_cdc_interfaces();
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }

        #if MICROPY_PY_OS_DUPTERM
        int dupterm_c = mp_uos_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        #endif
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (tud_cdc_connected()) {
        for (size_t i = 0; i < len;) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            int timeout = 0;
            // Wait with a max of USC_CDC_TIMEOUT ms
            while (n > tud_cdc_write_available() && timeout++ < MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                MICROPY_EVENT_POLL_HOOK
            }
            if (timeout >= MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                break;
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }
    }
    #if MICROPY_PY_OS_DUPTERM
    mp_uos_dupterm_tx_strn(str, len);
    #endif
}
