/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_USB_CDC_SERIAL_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_USB_CDC_SERIAL_H

#include "shared-module/usb_cdc/Serial.h"

extern const mp_obj_type_t usb_cdc_serial_type;

extern size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode);
extern size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode);

extern uint32_t common_hal_usb_cdc_serial_get_in_waiting(usb_cdc_serial_obj_t *self);
extern uint32_t common_hal_usb_cdc_serial_get_out_waiting(usb_cdc_serial_obj_t *self);

extern void common_hal_usb_cdc_serial_reset_input_buffer(usb_cdc_serial_obj_t *self);
extern uint32_t common_hal_usb_cdc_serial_reset_output_buffer(usb_cdc_serial_obj_t *self);

extern uint32_t common_hal_usb_cdc_serial_flush(usb_cdc_serial_obj_t *self);

extern bool common_hal_usb_cdc_serial_get_connected(usb_cdc_serial_obj_t *self);

extern mp_float_t common_hal_usb_cdc_serial_get_timeout(usb_cdc_serial_obj_t *self);
extern void common_hal_usb_cdc_serial_set_timeout(usb_cdc_serial_obj_t *self, mp_float_t timeout);

extern mp_float_t common_hal_usb_cdc_serial_get_write_timeout(usb_cdc_serial_obj_t *self);
extern void common_hal_usb_cdc_serial_set_write_timeout(usb_cdc_serial_obj_t *self, mp_float_t write_timeout);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_USB_CDC_SERIAL_H
