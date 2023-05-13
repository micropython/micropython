/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "common-hal/_bleio/CharacteristicBuffer.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"

// Characteristic buffer list of peripheral device
bleio_characteristic_buffer_obj_list_t bleio_characteristic_buffer_list;

bool characteristic_buffer_on_ble_evt(uint16_t attribute,
    uint8_t *data,
    uint16_t len) {

    uint16_t cindex = 0;
    for (cindex = 0; cindex < bleio_characteristic_buffer_list.len; cindex++) {
        if (bleio_characteristic_buffer_list.data[cindex] != NULL &&
            bleio_characteristic_buffer_list.data[cindex]->characteristic->handle == attribute) {
            taskENTER_CRITICAL();
            if (bleio_characteristic_buffer_list.data[cindex]->watch_for_interrupt_char) {
                for (uint16_t i = 0; i < len; i++) {
                    if (data[i] == mp_interrupt_char) {
                        mp_sched_keyboard_interrupt();
                    } else {
                        ringbuf_put(&bleio_characteristic_buffer_list.data[cindex]->ringbuf, data[i]);
                    }
                }
            } else {
                ringbuf_put_n(&bleio_characteristic_buffer_list.data[cindex]->ringbuf,data, len);
            }
            taskEXIT_CRITICAL();

            return true;
        }
    }
    return false;
}

void _common_hal_bleio_characteristic_buffer_construct(
    bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    uint8_t *buffer, size_t buffer_size,
    void *static_handler_entry,
    bool watch_for_interrupt_char) {

    self->characteristic = characteristic;
    self->timeout_ms = timeout * 1000;
    self->watch_for_interrupt_char = watch_for_interrupt_char;
    ringbuf_init(&self->ringbuf, buffer, buffer_size);
}

void common_hal_bleio_characteristic_buffer_construct(
    bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    size_t buffer_size) {

    uint8_t *buffer = m_malloc(buffer_size, true);
    _common_hal_bleio_characteristic_buffer_construct(self,
        characteristic,
        timeout,
        buffer,
        buffer_size,
        NULL,
        false);

    bleio_characteristic_buffer_list.data[bleio_characteristic_buffer_list.len] = self;
    bleio_characteristic_buffer_list.len++;
}

uint32_t common_hal_bleio_characteristic_buffer_read(
    bleio_characteristic_buffer_obj_t *self,
    uint8_t *data,
    size_t len,
    int *errcode) {

    uint64_t start_ticks = supervisor_ticks_ms64();
    // Wait for all bytes received or timeout
    while ((ringbuf_num_filled(&self->ringbuf) < len) &&
           (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }
    taskENTER_CRITICAL();
    uint32_t num_bytes_read = ringbuf_get_n(&self->ringbuf, data, len);
    taskEXIT_CRITICAL();

    return num_bytes_read;
}

uint32_t common_hal_bleio_characteristic_buffer_rx_characters_available(
    bleio_characteristic_buffer_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_bleio_characteristic_buffer_clear_rx_buffer(
    bleio_characteristic_buffer_obj_t *self) {
    taskENTER_CRITICAL();
    ringbuf_clear(&self->ringbuf);
    taskEXIT_CRITICAL();
}

bool common_hal_bleio_characteristic_buffer_deinited(
    bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (!common_hal_bleio_characteristic_buffer_deinited(self)) {
        self->characteristic = NULL;
        ringbuf_deinit(&self->ringbuf);
    }
}

bool common_hal_bleio_characteristic_buffer_connected(
    bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic != NULL &&
           self->characteristic->service != NULL &&
           (!self->characteristic->service->is_remote ||
               (self->characteristic->service->connection != MP_OBJ_NULL &&
                   common_hal_bleio_connection_get_connected(self->characteristic->service->connection)));
}

void reset_characteristic_buffer_list() {
    // Remove characteristic_buffer list
    memset(bleio_characteristic_buffer_list.data, 0,
        sizeof(bleio_characteristic_buffer_list.data));
    bleio_characteristic_buffer_list.len = 0;
}
