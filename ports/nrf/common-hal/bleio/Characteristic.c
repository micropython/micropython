/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "ble_drv.h"
#include "shared-module/bleio/Characteristic.h"

void data_callback(bleio_characteristic_obj_t *self, uint16_t length, uint8_t *data) {
    self->value_data = mp_obj_new_bytearray(length, data);
}

void common_hal_bleio_characteristic_read_value(bleio_characteristic_obj_t *self) {
    ble_drv_attr_c_read(self, data_callback);
}

void common_hal_bleio_characteristic_write_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    ubluepy_peripheral_obj_t *peripheral = MP_OBJ_TO_PTR(self->service->periph);
    ubluepy_role_type_t role = peripheral->role;

    if (role == UBLUEPY_ROLE_PERIPHERAL) {
        // TODO: Add indications
        if (self->props.notify) {
            ble_drv_attr_s_notify(self, bufinfo);
        } else {
            ble_drv_attr_s_write(self, bufinfo);
        }
    } else {
        ble_drv_attr_c_write(self, bufinfo);
    }

}
