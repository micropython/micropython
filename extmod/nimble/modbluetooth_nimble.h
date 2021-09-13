/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jim Mussared
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

#ifndef MICROPY_INCLUDED_EXTMOD_NIMBLE_MODBLUETOOTH_NIMBLE_H
#define MICROPY_INCLUDED_EXTMOD_NIMBLE_MODBLUETOOTH_NIMBLE_H

#include "extmod/modbluetooth.h"

#define MP_BLUETOOTH_NIMBLE_MAX_SERVICES (8)

typedef struct _mp_bluetooth_nimble_root_pointers_t {
    // Characteristic (and descriptor) value storage.
    mp_gatts_db_t gatts_db;

    // Pending service definitions.
    size_t n_services;
    struct ble_gatt_svc_def *services[MP_BLUETOOTH_NIMBLE_MAX_SERVICES];

    #if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
    // L2CAP channels.
    struct _mp_bluetooth_nimble_l2cap_channel_t *l2cap_chan;
    bool l2cap_listening;
    #endif
} mp_bluetooth_nimble_root_pointers_t;

enum {
    MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF,
    MP_BLUETOOTH_NIMBLE_BLE_STATE_STARTING,
    MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC,
    MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE,
    MP_BLUETOOTH_NIMBLE_BLE_STATE_STOPPING,
};

extern volatile int mp_bluetooth_nimble_ble_state;

// --- Optionally provided by the MicroPython port. ---------------------------
// (default implementations provided by modbluetooth_nimble.c)

// Tell the port to init the UART and start the HCI controller.
void mp_bluetooth_nimble_port_hci_init(void);

// Tell the port to deinit the UART and shutdown the HCI controller.
void mp_bluetooth_nimble_port_hci_deinit(void);

// Tell the port to run its background task (i.e. poll the UART and pump events).
void mp_bluetooth_nimble_port_start(void);

// Tell the port to stop its background task.
void mp_bluetooth_nimble_port_shutdown(void);

// --- Called by the HCI UART layer to let us know when packets have been sent.
void mp_bluetooth_nimble_sent_hci_packet(void);


#endif // MICROPY_INCLUDED_EXTMOD_NIMBLE_MODBLUETOOTH_NIMBLE_H
