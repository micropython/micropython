/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MPBTHCIPORT_H
#define MICROPY_INCLUDED_STM32_MPBTHCIPORT_H

#include "boardctrl.h"

// Initialise the HCI subsystem (should be called once, early on).
void mp_bluetooth_hci_init(void);

// Default implementations of poll functions (a board can override them).
void mp_bluetooth_hci_poll_now_default(void);
void mp_bluetooth_hci_poll_in_ms_default(uint32_t ms);

// Call this to poll the HCI now.
static inline void mp_bluetooth_hci_poll_now(void) {
    MICROPY_BOARD_BT_HCI_POLL_NOW();
}

// Call this to poll the HCI after a certain timeout.
static inline void mp_bluetooth_hci_poll_in_ms(uint32_t ms) {
    MICROPY_BOARD_BT_HCI_POLL_IN_MS(ms);
}

// Must be provided by the stack bindings (e.g. mpnimbleport.c or mpbtstackport.c).
// Request new data from the uart and pass to the stack, and run pending events/callouts.
// This is a low-level function and should not be called directly, use
// mp_bluetooth_hci_poll_now/mp_bluetooth_hci_poll_in_ms instead.
void mp_bluetooth_hci_poll(void);

#endif // MICROPY_INCLUDED_STM32_MPBTHCIPORT_H
