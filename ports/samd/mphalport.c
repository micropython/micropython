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

#if MICROPY_KBD_EXCEPTION
int mp_interrupt_char = -1;

void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    (void)itf;
    (void)wanted_char;
    tud_cdc_read_char(); // discard interrupt char
    mp_sched_keyboard_interrupt();
}

void mp_hal_set_interrupt_char(int c) {
    mp_interrupt_char = c;
    tud_cdc_set_wanted_char(c);
}

#endif

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
        uint32_t start = mp_hal_ticks_us();
        #if defined(MCU_SAMD21)
        // SAMD21 counter has effective 32 bit width
        while ((mp_hal_ticks_us() - start) < us) {
        }
        #elif defined(MCU_SAMD51)
        // SAMD51 counter has effective 29 bit width
        while (((mp_hal_ticks_us() - start) & (MICROPY_PY_UTIME_TICKS_PERIOD - 1)) < us) {
        }
        #endif
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (tud_cdc_connected() && tud_cdc_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (tud_cdc_connected() && tud_cdc_available()) {
            uint8_t buf[1];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            if (count) {
                return buf[0];
            }
        }
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
            while (n > tud_cdc_write_available()) {
                MICROPY_EVENT_POLL_HOOK
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }
    }
}
