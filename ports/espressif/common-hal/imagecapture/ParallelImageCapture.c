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
    mp_arg_validate_int_range(data_count, 8, 16, MP_QSTR_data_count);

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
    cam_deinit();

    self->buffer1 = NULL;
    self->buffer2 = NULL;

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

void common_hal_imagecapture_parallelimagecapture_continuous_capture_start(imagecapture_parallelimagecapture_obj_t *self, mp_obj_t buffer1, mp_obj_t buffer2) {
    if (buffer1 == self->buffer1 && buffer2 == self->buffer2) {
        return;
    }

    mp_buffer_info_t bufinfo1, bufinfo2 = {};
    mp_get_buffer_raise(buffer1, &bufinfo1, MP_BUFFER_RW);
    if (buffer2 != mp_const_none) {
        mp_get_buffer_raise(buffer2, &bufinfo2, MP_BUFFER_RW);
        if (bufinfo1.len != bufinfo2.len) {
            mp_raise_ValueError(MP_ERROR_TEXT("Buffers must be same size"));
        }
    }

    self->buffer1 = buffer1;
    self->buffer2 = buffer2;


    cam_deinit();
    self->config.size = bufinfo1.len / 2; // count is in pixels
    self->config.frame1_buffer = bufinfo1.buf;
    self->config.frame2_buffer = bufinfo2.buf;
    self->buffer_to_give = NULL;

    cam_init(&self->config);
    cam_start();
}

void common_hal_imagecapture_parallelimagecapture_continuous_capture_stop(imagecapture_parallelimagecapture_obj_t *self) {
    cam_deinit();
    self->buffer1 = self->buffer2 = NULL;
    self->buffer_to_give = NULL;
}

STATIC void common_hal_imagecapture_parallelimagecapture_continuous_capture_give_frame(imagecapture_parallelimagecapture_obj_t *self) {
    if (self->buffer_to_give) {
        cam_give(self->buffer_to_give);
        self->buffer_to_give = NULL;
    }
}

mp_obj_t common_hal_imagecapture_parallelimagecapture_continuous_capture_get_frame(imagecapture_parallelimagecapture_obj_t *self) {
    if (self->buffer1 == NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No capture in progress"));
    }
    common_hal_imagecapture_parallelimagecapture_continuous_capture_give_frame(self);

    while (!cam_ready()) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            return mp_const_none;
        }
    }

    cam_take(&self->buffer_to_give);

    if (self->buffer_to_give == self->config.frame1_buffer) {
        return self->buffer1;
    }
    if (self->buffer_to_give == self->config.frame2_buffer) {
        return self->buffer2;
    }

    return mp_const_none;  // should be unreachable
}

void common_hal_imagecapture_parallelimagecapture_singleshot_capture(imagecapture_parallelimagecapture_obj_t *self, mp_obj_t buffer) {
    common_hal_imagecapture_parallelimagecapture_continuous_capture_start(self, buffer, mp_const_none);
    common_hal_imagecapture_parallelimagecapture_continuous_capture_get_frame(self);
}
