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

#include "shared-module/usb_cdc/Serial.h"
#include "tusb.h"

size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    return tud_cdc_n_read(self->idx, data, len);
}

uint32_t common_hal_usb_cdc_serial_bytes_available(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_available(self->idx);
}

size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    return tud_cdc_n_write(self->idx, data, len);
}

bool common_hal_usb_cdc_serial_ready_to_tx(usb_cdc_serial_obj_t *self) {
    return tud_cdc_n_connected(self->idx);
}
