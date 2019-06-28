/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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
#include "nrf_nvic.h"

#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "tick.h"

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
            uint8_t is_nested_critical_region;
            sd_nvic_critical_region_enter(&is_nested_critical_region);
            for (size_t i = 0; i < evt_write->len; i++) {
                ringbuf_put(&self->ringbuf, evt_write->data[i]);
            }
            sd_nvic_critical_region_exit(is_nested_critical_region);
            break;
        }
    }
    }

}

// Assumes that timeout and buffer_size have been validated before call.
void common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self,
                                                      bleio_characteristic_obj_t *characteristic,
                                                      mp_float_t timeout,
                                                      size_t buffer_size) {

    self->characteristic = characteristic;
    self->timeout_ms = timeout * 1000;
    // This is a macro.
    // true means long-lived, so it won't be moved.
    ringbuf_alloc(&self->ringbuf, buffer_size, true);

    ble_drv_add_event_handler(characteristic_buffer_on_ble_evt, self);

}

int common_hal_bleio_characteristic_buffer_read(bleio_characteristic_buffer_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    uint64_t start_ticks = ticks_ms;

    // Wait for all bytes received or timeout
    while ( (ringbuf_count(&self->ringbuf) < len) && (ticks_ms - start_ticks < self->timeout_ms) ) {
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP ;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if ( mp_hal_is_interrupted() ) {
            return 0;
        }
#endif
    }

    // Copy received data. Lock out write interrupt handler while copying.
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);

    size_t rx_bytes = MIN(ringbuf_count(&self->ringbuf), len);
    for ( size_t i = 0; i < rx_bytes; i++ ) {
        data[i] = ringbuf_get(&self->ringbuf);
    }

    // Writes now OK.
    sd_nvic_critical_region_exit(is_nested_critical_region);

    return rx_bytes;
}

uint32_t common_hal_bleio_characteristic_buffer_rx_characters_available(bleio_characteristic_buffer_obj_t *self) {
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);
    uint16_t count = ringbuf_count(&self->ringbuf);
    sd_nvic_critical_region_exit(is_nested_critical_region);
    return count;
}

void common_hal_bleio_characteristic_buffer_clear_rx_buffer(bleio_characteristic_buffer_obj_t *self) {
    // prevent conflict with uart irq
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);
    ringbuf_clear(&self->ringbuf);
    sd_nvic_critical_region_exit(is_nested_critical_region);
}

bool common_hal_bleio_characteristic_buffer_deinited(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (!common_hal_bleio_characteristic_buffer_deinited(self)) {
        ble_drv_remove_event_handler(characteristic_buffer_on_ble_evt, self);
    }
}

bool common_hal_bleio_characteristic_buffer_connected(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic != NULL &&
        self->characteristic->service != NULL &&
        self->characteristic->service->device != NULL &&
        common_hal_bleio_device_get_conn_handle(self->characteristic->service->device) != BLE_CONN_HANDLE_INVALID;
}
