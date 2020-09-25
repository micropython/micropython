/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jim Mussared
 * Copyright (c) 2020 Damien P. George
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
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

#include "host/ble_hs.h"
#include "nimble/nimble_npl.h"

#include "extmod/mpbthci.h"
#include "extmod/nimble/modbluetooth_nimble.h"
#include "extmod/nimble/hal/hal_uart.h"

// This implements the Nimble "background task". It's called at PENDSV
// priority, either every 128ms or whenever there's UART data available.
// Because it's called via PENDSV, you can implicitly consider that it
// is surrounded by MICROPY_PY_BLUETOOTH_ENTER / MICROPY_PY_BLUETOOTH_EXIT.
void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        // Ask NimBLE to process UART data.
        mp_bluetooth_nimble_hci_uart_process();

        // Run pending background operations and events, but only after HCI sync.
        mp_bluetooth_nimble_os_callout_process();
        mp_bluetooth_nimble_os_eventq_run_all();
    }
}

// --- Port-specific helpers for the generic NimBLE bindings. -----------------

void mp_bluetooth_nimble_hci_uart_wfi(void) {
    __WFI();
}

uint32_t mp_bluetooth_nimble_hci_uart_enter_critical(void) {
    MICROPY_PY_BLUETOOTH_ENTER
    return atomic_state;
}

void mp_bluetooth_nimble_hci_uart_exit_critical(uint32_t atomic_state) {
    MICROPY_PY_BLUETOOTH_EXIT
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
