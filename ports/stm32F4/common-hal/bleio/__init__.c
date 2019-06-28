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

#include "shared-bindings/bleio/__init__.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/Peripheral.h"
#include "common-hal/bleio/__init__.h"

// Turn off BLE on a reset or reload.
void bleio_reset() {
    if (common_hal_bleio_adapter_get_enabled()) {
        common_hal_bleio_adapter_set_enabled(false);
    }
}

// The singleton bleio.Adapter object, bound to bleio.adapter
// It currently only has properties and no state
const super_adapter_obj_t common_hal_bleio_adapter_obj = {
    .base = {
        .type = &bleio_adapter_type,
    },
};

gatt_role_t common_hal_bleio_device_get_gatt_role(mp_obj_t device) {
    if (MP_OBJ_IS_TYPE(device, &bleio_peripheral_type)) {
        return ((bleio_peripheral_obj_t*) MP_OBJ_TO_PTR(device))->gatt_role;
// Does not exist yet.
//    } else if (MP_OBJ_IS_TYPE(device, &bleio_central_type)) {
//        return ((bleio_central_obj_t*) MP_OBJ_TO_PTR(device))->gatt_role;
    } else {
        return GATT_ROLE_NONE;
    }
}

uint16_t common_hal_bleio_device_get_conn_handle(mp_obj_t device) {
    if (MP_OBJ_IS_TYPE(device, &bleio_peripheral_type)) {
        return ((bleio_peripheral_obj_t*) MP_OBJ_TO_PTR(device))->conn_handle;
// Does not exist yet.
//    } else if (MP_OBJ_IS_TYPE(device, &bleio_central_type)) {
//        return ((bleio_central_obj_t*) MP_OBJ_TO_PTR(device))->conn_handle;
    } else {
        return 0;
    }
}
