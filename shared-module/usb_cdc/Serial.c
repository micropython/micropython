/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "lib/utils/interrupt_char.h"
#include "shared-module/usb_cdc/Serial.h"
#include "supervisor/shared/tick.h"

#include "tusb.h"

size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->timeout < 0.0f) {
        while (tud_cdc_n_available(self->idx) < len) {
            RUN_BACKGROUND_TASKS;
            if (mp_hal_is_interrupted()) {
                return 0;
            }
        }
    } else if (self->timeout > 0.0f) {
        uint64_t timeout_ms = self->timeout * 1000;
        uint64_t start_ticks = supervisor_ticks_ms64();
        while (tud_cdc_n_available(self->idx) < len &&
               supervisor_ticks_ms64() - start_ticks <= timeout_ms) {
            RUN_BACKGROUND_TASKS;
            if (mp_hal_is_interrupted()) {
                return 0;
            }
        }
    }
    // Timeout of 0.0f falls through to here with no waiting or unnecessary calculation.
    return tud_cdc_n_read(self->idx, data, len);
}

size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    uint32_t num_written = tud_cdc_n_write(self->idx, data, len);
    tud_cdc_n_write_flush(self->idx);
    return num_written;
}

uint32_t common_hal_usb_cdc_serial_get_in_waiting(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_available(self->idx);
}

uint32_t common_hal_usb_cdc_serial_get_out_waiting(usb_cdc_serial_obj_t *self) {
    // Return number of FIFO bytes currently occupied.
    return CFG_TUD_CDC_TX_BUFSIZE - tud_cdc_n_write_available(self->idx);
}

void common_hal_usb_cdc_serial_reset_input_buffer(usb_cdc_serial_obj_t *self) {
    tud_cdc_n_read_flush(self->idx);
}

uint32_t common_hal_usb_cdc_serial_reset_output_buffer(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_write_clear(self->idx);
}

uint32_t common_hal_usb_cdc_serial_flush(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_write_flush(self->idx);
}

bool common_hal_usb_cdc_serial_get_connected(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_connected(self->idx);
}

mp_float_t common_hal_usb_cdc_serial_get_timeout(usb_cdc_serial_obj_t *self) {
    return self->timeout;
}

void common_hal_usb_cdc_serial_set_timeout(usb_cdc_serial_obj_t *self, mp_float_t timeout) {
    self->timeout = timeout;
}
