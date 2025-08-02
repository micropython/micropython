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
#include "shared/tinyusb/mp_usbd.h"
#include "shared/tinyusb/mp_usbd_cdc.h"
#include "extmod/misc.h"
#include "samd_soc.h"
#include "tusb.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 128
#endif

extern volatile uint32_t ticks_us64_upper;

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0 };

// Explicitly run the USB stack in case the scheduler is locked (eg we are in an
// interrupt handler) and there is in/out data pending on the USB CDC interface.
#define MICROPY_EVENT_POLL_HOOK_WITH_USB \
    do { \
        MICROPY_EVENT_POLL_HOOK; \
        mp_usbd_task(); \
    } while (0)

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
    uint32_t t0 = systick_ms;
    while (systick_ms - t0 < ms) {
        MICROPY_EVENT_POLL_HOOK
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
    uint32_t us64_lower;
    uint8_t intflag;
    __disable_irq();
    uint32_t us64_upper = ticks_us64_upper;
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
    return ((uint64_t)us64_upper << 31) | (us64_lower >> 1);
    #elif defined(MCU_SAMD51)
    return ((uint64_t)us64_upper << 28) | (us64_lower >> 4);
    #endif

}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_HW_USB_CDC
    ret |= mp_usbd_cdc_poll_interfaces(poll_flags);
    #endif
    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_os_dupterm_poll(poll_flags);
    #endif
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if MICROPY_HW_USB_CDC
        mp_usbd_cdc_poll_interfaces(0);
        #endif
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }

        #if MICROPY_PY_OS_DUPTERM
        int dupterm_c = mp_os_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        #endif
        MICROPY_EVENT_POLL_HOOK_WITH_USB;
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    bool did_write = false;
    #if MICROPY_HW_USB_CDC
    mp_uint_t cdc_res = mp_usbd_cdc_tx_strn(str, len);
    if (cdc_res > 0) {
        did_write = true;
        ret = MIN(cdc_res, ret);
    }
    #endif
    #if MICROPY_PY_OS_DUPTERM
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        did_write = true;
        ret = MIN((mp_uint_t)dupterm_res, ret);
    }
    #endif
    return did_write ? ret : 0;
}
