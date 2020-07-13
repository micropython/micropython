/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#ifndef MICROPY_INCLUDED_EXTMOD_BTSTACK_MODBLUETOOTH_BTSTACK_H
#define MICROPY_INCLUDED_EXTMOD_BTSTACK_MODBLUETOOTH_BTSTACK_H

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "extmod/modbluetooth.h"

#include "lib/btstack/src/btstack.h"

typedef struct _mp_bluetooth_btstack_root_pointers_t {
    // This stores both the advertising data and the scan response data, concatenated together.
    uint8_t *adv_data;
    // Total length of both.
    size_t adv_data_alloc;

    // Characteristic (and descriptor) value storage.
    mp_gatts_db_t gatts_db;

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    // Registration for notify/indicate events.
    gatt_client_notification_t notification;
    #endif
} mp_bluetooth_btstack_root_pointers_t;

enum {
    MP_BLUETOOTH_BTSTACK_STATE_OFF,
    MP_BLUETOOTH_BTSTACK_STATE_STARTING,
    MP_BLUETOOTH_BTSTACK_STATE_ACTIVE,
    MP_BLUETOOTH_BTSTACK_STATE_TIMEOUT,
};

extern volatile int mp_bluetooth_btstack_state;

void mp_bluetooth_btstack_port_init(void);
void mp_bluetooth_btstack_port_deinit(void);
void mp_bluetooth_btstack_port_start(void);

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#endif // MICROPY_INCLUDED_EXTMOD_BTSTACK_MODBLUETOOTH_BTSTACK_H
