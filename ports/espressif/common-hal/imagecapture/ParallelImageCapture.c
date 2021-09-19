/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/audiobusio/__init__.h"
#include "common-hal/imagecapture/ParallelImageCapture.h"
#include "cam.h"

void common_hal_imagecapture_parallelimagecapture_construct(imagecapture_parallelimagecapture_obj_t *self,
    const uint8_t data_pins[],
    uint8_t data_count,
    const mcu_pin_obj_t *data_clock,
    const mcu_pin_obj_t *vertical_sync,
    const mcu_pin_obj_t *horizontal_reference) {

    // only 8 bits is supported at present
    if (data_count < 8 || data_count > 16) {
        mp_raise_ValueError_varg(translate("%q must be between %d and %d"), MP_QSTR_data_count, 8, 16);
    }

    // This will throw if unsuccessful.  Everything following is guaranteed to succeed.
    port_i2s_allocate_i2s0();

    claim_pin(data_clock);
    claim_pin(vertical_sync);
    claim_pin(horizontal_reference);

    self->data_count = data_count;
    self->data_clock = data_clock->number;
    self->vertical_sync = vertical_sync->number;
    self->horizontal_reference = horizontal_reference->number;

    self->config = (cam_config_t) {
        .bit_width = data_count,
        .pin = {
            .pclk = data_clock->number,
            .vsync = vertical_sync->number,
            .hsync = horizontal_reference->number,
        },
        .vsync_invert = true,
        .hsync_invert = false,
        .size = 0,
        .max_buffer_size = 8 * 1024,
        .task_stack = 1024,
        .task_pri = configMAX_PRIORITIES
    };

    for (int i = 0; i < data_count; i++) {
        claim_pin_number(data_pins[i]);
        self->config.pin_data[i] = data_pins[i];
    }
}

void common_hal_imagecapture_parallelimagecapture_deinit(imagecapture_parallelimagecapture_obj_t *self) {
    reset_pin_number(self->data_clock);
    self->data_clock = NO_PIN;

    reset_pin_number(self->vertical_sync);
    self->vertical_sync = NO_PIN;

    reset_pin_number(self->horizontal_reference);
    self->horizontal_reference = NO_PIN;

    for (int i = 0; i < self->data_count; i++) {
        if (self->config.pin_data[i] != NO_PIN) {
            reset_pin_number(self->config.pin_data[i]);
            self->config.pin_data[i] = NO_PIN;
        }
    }

    port_i2s_reset_instance(0);
}

bool common_hal_imagecapture_parallelimagecapture_deinited(imagecapture_parallelimagecapture_obj_t *self) {
    return self->data_clock == NO_PIN;
}

void common_hal_imagecapture_parallelimagecapture_capture(imagecapture_parallelimagecapture_obj_t *self, void *buffer, size_t bufsize) {
    size_t size = bufsize / 2; // count is in pixels
    if (size != self->config.size || buffer != self->config.frame1_buffer) {
        cam_deinit();
        self->config.size = bufsize / 2; // count is in pixels(?)
        self->config.frame1_buffer = buffer;

        cam_init(&self->config);
    } else {
        cam_give(buffer);
    }
    cam_start();

    while (!cam_ready()) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            self->config.size = 0; // force re-init next time
            cam_stop();
            return;
        }
    }

    uint8_t *unused;
    cam_take(&unused); // this just "returns" buffer

    cam_stop();
}
