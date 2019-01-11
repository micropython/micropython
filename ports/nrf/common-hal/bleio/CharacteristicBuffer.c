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

#include <string.h>
#include <stdio.h>

#include "ble_drv.h"
#include "ble_gatts.h"
#include "nrf_soc.h"

#include "py/runtime.h"

#include "common-hal/bleio/__init__.h"
#include "common-hal/bleio/CharacteristicBuffer.h"

STATIC void characteristic_buffer_on_ble_evt(ble_evt_t *ble_evt, void *param) {
    bleio_characteristic_buffer_obj_t *self = (bleio_characteristic_buffer_obj_t *) param;
    switch (ble_evt->header.evt_id) {
    case BLE_GATTS_EVT_WRITE: {
        ble_gatts_evt_write_t *evt_write = &ble_evt->evt.gatts_evt.params.write;
        // Event handle must match the handle for my characteristic.
        if (evt_write->handle == self->characteristic->handle) {
            // Push all the data onto the ring buffer.
            for (size_t i = 0; i < evt_write->len; i++) {
                ringbuf_put(&self->ringbuf, evt_write->data[i]);
            }
        break;
        }
    }
    }

}

// Assumes that buffer_size has been validated before call.
void common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic, size_t buffer_size) {

    self->characteristic = characteristic;
    // This is a macro.
    ringbuf_alloc(&self->ringbuf, buffer_size);

    ble_drv_add_event_handler(characteristic_buffer_on_ble_evt, self);

}

// Returns a uint8_t byte value, or -1 if no data is available.
int common_hal_bleio_characteristic_buffer_read(bleio_characteristic_buffer_obj_t *self) {
    return ringbuf_get(&self->ringbuf);
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    ble_drv_remove_event_handler(characteristic_buffer_on_ble_evt, self);
}
