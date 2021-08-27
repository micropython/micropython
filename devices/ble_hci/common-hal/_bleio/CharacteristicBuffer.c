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

#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "supervisor/shared/tick.h"
#include "common-hal/_bleio/CharacteristicBuffer.h"

// Push all the data onto the ring buffer. When the buffer is full, new bytes will be dropped.
STATIC void write_to_ringbuf(bleio_characteristic_buffer_obj_t *self, uint8_t *data, uint16_t len) {
    ringbuf_put_n(&self->ringbuf, data, len);
}

void bleio_characteristic_buffer_update(bleio_characteristic_buffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    write_to_ringbuf(self, bufinfo->buf, bufinfo->len);
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

    bleio_characteristic_set_observer(characteristic, self);
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

uint32_t common_hal_bleio_characteristic_buffer_rx_characters_available(bleio_characteristic_buffer_obj_t *self) {
    uint16_t count = ringbuf_num_filled(&self->ringbuf);
    return count;
}

void common_hal_bleio_characteristic_buffer_clear_rx_buffer(bleio_characteristic_buffer_obj_t *self) {
    ringbuf_clear(&self->ringbuf);
}

bool common_hal_bleio_characteristic_buffer_deinited(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (!common_hal_bleio_characteristic_buffer_deinited(self)) {
        bleio_characteristic_clear_observer(self->characteristic);
    }
}

bool common_hal_bleio_characteristic_buffer_connected(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic != NULL &&
           self->characteristic->service != NULL &&
           (!self->characteristic->service->is_remote ||
               (self->characteristic->service->connection != MP_OBJ_NULL &&
                   common_hal_bleio_connection_get_connected(self->characteristic->service->connection)));
}
