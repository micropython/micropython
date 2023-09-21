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
#include "extmod/nimble/transport/uart_ll.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)

extern pthread_mutex_t nimble_mutex;

bool mp_bluetooth_hci_active(void) {
    return mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;
}

bool mp_bluetooth_run_hci_uart(void) {
    if (!mp_bluetooth_hci_active()) {
        return false;
    }

    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        mp_bluetooth_nimble_hci_uart_process();
    }

    return true;
}

bool mp_bluetooth_run_host_stack(void) {
    if (!mp_bluetooth_hci_active()) {
        return false;
    }

    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        // Run any timers and pending events in the queue.
        mp_bluetooth_nimble_run_host_stack();
    }

    return true;
}

// Extra port-specific helpers.
void mp_bluetooth_nimble_hci_uart_wfi(void) {
    // pthread_mutex_lock(&nimble_mutex);
    // // This is called while NimBLE is waiting in ble_npl_sem_pend, i.e. waiting for an HCI ACK.
    // // Do not need to run events here, only processing incoming HCI data.
    // mp_bluetooth_nimble_hci_uart_process(false);
    // pthread_mutex_unlock(&nimble_mutex);
    // mp_bluetooth_hci_poll(true);

    sched_yield();
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
