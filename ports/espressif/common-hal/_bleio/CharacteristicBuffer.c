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

#include "shared/runtime/interrupt_char.h"
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "supervisor/shared/tick.h"
#include "common-hal/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"

STATIC int characteristic_buffer_on_ble_evt(struct ble_gap_event *event, void *param) {
    bleio_characteristic_buffer_obj_t *self = (bleio_characteristic_buffer_obj_t *)param;
    switch (event->type) {
        case BLE_GAP_EVENT_NOTIFY_RX: {
            // A remote service wrote to this characteristic.

            // Must be a notification, and event handle must match the handle for my characteristic.
            if (event->notify_rx.indication == 0 &&
                event->notify_rx.attr_handle == self->characteristic->handle) {
                const struct os_mbuf *m = event->notify_rx.om;
                while (m != NULL) {
                    ringbuf_put_n(&self->ringbuf, m->om_data, m->om_len);
                    m = SLIST_NEXT(m, om_next);
                }
            }
            break;
        }
        default:
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "Unhandled gap event %d\n", event->type);
            #endif
            return 0;
            break;
    }
    return 0;
}

void _common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    uint8_t *buffer, size_t buffer_size,
    void *static_handler_entry) {
    self->characteristic = characteristic;
    self->timeout_ms = timeout * 1000;
    ringbuf_init(&self->ringbuf, buffer, buffer_size);

    if (static_handler_entry != NULL) {
        ble_event_add_handler_entry((ble_event_handler_entry_t *)static_handler_entry, characteristic_buffer_on_ble_evt, self);
    } else {
        ble_event_add_handler(characteristic_buffer_on_ble_evt, self);
    }
}

// Assumes that timeout and buffer_size have been validated before call.
void common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    size_t buffer_size) {
    uint8_t *buffer = m_malloc(buffer_size, true);
    _common_hal_bleio_characteristic_buffer_construct(self, characteristic, timeout, buffer, buffer_size, NULL);
}

uint32_t common_hal_bleio_characteristic_buffer_read(bleio_characteristic_buffer_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Wait for all bytes received or timeout
    while ((ringbuf_num_filled(&self->ringbuf) < len) && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }

    uint32_t num_bytes_read = ringbuf_get_n(&self->ringbuf, data, len);

    return num_bytes_read;
}

// NOTE: The nRF port has protection around these operations because the ringbuf
// is filled from an interrupt. On ESP the ringbuf is filled from the BLE host
// task that won't interrupt us.

uint32_t common_hal_bleio_characteristic_buffer_rx_characters_available(bleio_characteristic_buffer_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_bleio_characteristic_buffer_clear_rx_buffer(bleio_characteristic_buffer_obj_t *self) {
    ringbuf_clear(&self->ringbuf);
}

bool common_hal_bleio_characteristic_buffer_deinited(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (!common_hal_bleio_characteristic_buffer_deinited(self)) {
        ble_event_remove_handler(characteristic_buffer_on_ble_evt, self);
        self->characteristic = NULL;
        ringbuf_deinit(&self->ringbuf);
    }
}

bool common_hal_bleio_characteristic_buffer_connected(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic != NULL &&
           self->characteristic->service != NULL &&
           (!self->characteristic->service->is_remote ||
               (self->characteristic->service->connection != MP_OBJ_NULL &&
                   common_hal_bleio_connection_get_connected(self->characteristic->service->connection)));
}
