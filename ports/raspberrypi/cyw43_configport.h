/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien P. George
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
#ifndef MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H
#define MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H

// The board-level config will be included here, so it can set some CYW43 values.
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "supervisor/port.h"

#include "sdk/src/rp2_common/pico_cyw43_driver/include/cyw43_configport.h"

#define CYW43_NETUTILS                  (1)

#if CIRCUITPY_USB
#include "supervisor/usb.h"
#define CYW43_EVENT_POLL_HOOK usb_background();
#else
#define CYW43_EVENT_POLL_HOOK
#endif

void cyw43_post_poll_hook(void);
extern volatile int cyw43_has_pending;

static inline void cyw43_yield(void) {
    uint32_t my_interrupts = save_and_disable_interrupts();
    if (!cyw43_has_pending) {
        port_idle_until_interrupt();
    }
    restore_interrupts(my_interrupts);
}

#endif // MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H
