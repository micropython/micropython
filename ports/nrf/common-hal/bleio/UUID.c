/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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
#include "common-hal/bleio/UUID.h"
#include "shared-bindings/bleio/Adapter.h"

#include "ble.h"
#include "ble_drv.h"
#include "nrf_error.h"

// If uuid128 is NULL, this is a Bluetooth SIG 16-bit UUID.
// If uuid128 is not NULL, it's a 128-bit (16-byte) UUID, with bytes 12 and 13 zero'd out, where
// the 16-bit part goes. Those 16 bits are passed in uuid16.
void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, uint32_t uuid16, uint8_t uuid128[]) {
    self->uuid16 = uuid16;
    self->uuid_vs_idx = 0;
    if (uuid128 != NULL) {
        ble_uuid128_t vs_uuid;
        memcpy(vs_uuid.uuid128, uuid128, sizeof(vs_uuid.uuid128));

        // Register this vendor-specific UUID. Bytes 12 and 13 will be zero.
        common_hal_bleio_adapter_set_enabled(true);
        const uint32_t err_code = sd_ble_uuid_vs_add(&vs_uuid, &self->uuid_vs_idx);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg(translate("Could not register Vendor-Specific UUID"));
        }
    }
}

void common_hal_bleio_uuid_print(bleio_uuid_obj_t *self, const mp_print_t *print) {
    if (self->uuid_vs_idx != 0) {
        mp_printf(print, "UUID(uuid16=0x%04x, uuid128_handle=" HEX2_FMT ")",
                  self->uuid16, self->uuid_vs_idx);
    } else {
        mp_printf(print, "UUID(0x%04x)", self->uuid16);
    }
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->uuid_vs_idx != 0 ? 128 : 16;
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->uuid16;
}

// Returns 0 if there is no handle, otherwise returns a non-zero index.
uint32_t common_hal_bleio_uuid_get_uuid128_handle(bleio_uuid_obj_t *self) {
    return self->uuid_vs_idx;
}


void bleio_uuid_construct_from_nrf_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_uuid) {
    if (nrf_uuid->type == BLE_UUID_TYPE_UNKNOWN) {
        mp_raise_RuntimeError(translate("Unexpected nrfx uuid type"));
    }

    self->uuid16 = nrf_uuid->uuid;
    self->uuid_vs_idx = nrf_uuid->type == BLE_UUID_TYPE_BLE ? 0 : nrf_uuid->type;
}

// Fill in a ble_uuid_t from my values.
void bleio_uuid_convert_to_nrf_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_uuid) {
    nrf_uuid->uuid = self->uuid16;
    nrf_uuid->type = self->uuid_vs_idx == 0 ? BLE_UUID_TYPE_BLE : self->uuid_vs_idx;
}
