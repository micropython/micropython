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
#include "py/stream.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
#define DEBUG_printf(...) // printf("mpnimbleport.c: " __VA_ARGS__)

#include "host/ble_hs.h"
#include "nimble/nimble_npl.h"

#include "extmod/mpbthci.h"
#include "extmod/modbluetooth.h"
#include "extmod/nimble/modbluetooth_nimble.h"
#include "extmod/nimble/hal/hal_uart.h"
#include "mpbthciport.h"

// Get any pending data from the UART and send it to NimBLE's HCI buffers.
// Any further processing by NimBLE will be run via its event queue.
void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        // DEBUG_printf("mp_bluetooth_hci_poll_uart %d\n", mp_bluetooth_nimble_ble_state);

        // Run any timers.
        mp_bluetooth_nimble_os_callout_process();

        // Process incoming UART data, and run events as they are generated.
        mp_bluetooth_nimble_hci_uart_process(true);

        // Run any remaining events (e.g. if there was no UART data).
        mp_bluetooth_nimble_os_eventq_run_all();
    }

    if (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        // Call this function again in 128ms to check for new events.
        // TODO: improve this by only calling back when needed.
        mp_bluetooth_hci_poll_in_ms(128);
    }
}

// --- Port-specific helpers for the generic NimBLE bindings. -----------------

void mp_bluetooth_nimble_hci_uart_wfi(void) {
    #if defined(__WFI)
    __WFI();
    #endif
    // This is called while NimBLE is waiting in ble_npl_sem_pend, i.e. waiting for an HCI ACK.
    // Do not need to run events here (it must not invoke Python code), only processing incoming HCI data.
    mp_bluetooth_nimble_hci_uart_process(false);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
