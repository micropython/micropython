/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared
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

#include "nimble/nimble_npl.h"

#include "extmod/nimble/modbluetooth_nimble.h"
#include "extmod/nimble/hal/hal_uart.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)

// Called by the UART polling thread in mpbthciport.c.
bool mp_bluetooth_hci_poll(void) {
    // DEBUG_printf("mp_bluetooth_hci_poll (unix nimble) %d\n", mp_bluetooth_nimble_ble_state);

    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        DEBUG_printf("mp_bluetooth_hci_poll (unix nimble) -- shutdown\n");
        return false;
    }

    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {

        // Pretend like we're running in IRQ context (i.e. other things can't be running at the same time).
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

        // Ask NimBLE to process UART data.
        mp_bluetooth_nimble_hci_uart_process();

        // Run pending background operations and events, but only after HCI sync.
        mp_bluetooth_nimble_os_callout_process();
        mp_bluetooth_nimble_os_eventq_run_all();

        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }

    return true;
}

// Extra port-specific helpers.
void mp_bluetooth_nimble_hci_uart_wfi(void) {
    // DEBUG_printf("mp_bluetooth_nimble_hci_uart_wfi\n");
    // TODO: this should do a select() on the uart_fd.
}

uint32_t mp_bluetooth_nimble_hci_uart_enter_critical(void) {
    // DEBUG_printf("mp_bluetooth_nimble_hci_uart_enter_critical\n");
    MICROPY_PY_BLUETOOTH_ENTER
    return atomic_state; // Always 0xffffffff
}

void mp_bluetooth_nimble_hci_uart_exit_critical(uint32_t atomic_state) {
    MICROPY_PY_BLUETOOTH_EXIT
    // DEBUG_printf("mp_bluetooth_nimble_hci_uart_exit_critical\n");
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
