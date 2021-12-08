/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/bluetooth/bluetooth.h"

// UUID shared by all cccd's.
bleio_uuid_obj_t cccd_uuid;

bool vm_used_ble;

// void check_sec_status(uint8_t sec_status) {
//     if (sec_status == BLE_GAP_SEC_STATUS_SUCCESS) {
//         return;
//     }

//     switch (sec_status) {
//         case BLE_GAP_SEC_STATUS_UNSPECIFIED:
//             mp_raise_bleio_SecurityError(translate("Unspecified issue. Can be that the pairing prompt on the other device was declined or ignored."));
//             return;
//         default:
//             mp_raise_bleio_SecurityError(translate("Unknown security error: 0x%04x"), sec_status);
//     }
// }

// Turn off BLE on a reset or reload.
void bleio_reset() {
    // Create a UUID object for all CCCD's.
    cccd_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&cccd_uuid, BLE_UUID_CCCD, NULL);

    bleio_hci_reset();

    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    if (!vm_used_ble) {
        // No user-code BLE operations were done, so we can maintain the supervisor state.
        return;
    }
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    common_hal_bleio_adapter_obj.allocated = false;

    bleio_set_adapter(mp_const_none);

    // FIX bonding_reset();
    supervisor_start_bluetooth();
}

// The singleton _bleio.Adapter object, bound to _bleio.adapter
bleio_adapter_obj_t common_hal_bleio_adapter_obj = {
    .base = {
        .type = &bleio_adapter_type,
    },
};

bleio_adapter_obj_t *common_hal_bleio_allocate_adapter_or_raise(void) {
    if (common_hal_bleio_adapter_obj.allocated) {
        mp_raise_RuntimeError(translate("Too many Adapters"));
    }
    return &common_hal_bleio_adapter_obj;
}

void common_hal_bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_ConnectionError(translate("Not connected"));
    }
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}


void bleio_hci_background(void) {
    bleio_adapter_background(&common_hal_bleio_adapter_obj);
}
