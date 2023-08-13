/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_USB_CORE_DEVICE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_USB_CORE_DEVICE_H

#include "py/objarray.h"

#include "shared-module/usb/core/Device.h"

extern const mp_obj_type_t usb_core_device_type;

bool common_hal_usb_core_device_construct(usb_core_device_obj_t *self, uint8_t device_number);
uint16_t common_hal_usb_core_device_get_idVendor(usb_core_device_obj_t *self);
uint16_t common_hal_usb_core_device_get_idProduct(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_serial_number(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_product(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_manufacturer(usb_core_device_obj_t *self);
void common_hal_usb_core_device_set_configuration(usb_core_device_obj_t *self, mp_int_t configuration);
mp_int_t common_hal_usb_core_device_write(usb_core_device_obj_t *self, mp_int_t endpoint, const uint8_t *buffer, mp_int_t len, mp_int_t timeout);
mp_int_t common_hal_usb_core_device_read(usb_core_device_obj_t *self, mp_int_t endpoint, uint8_t *buffer, mp_int_t len, mp_int_t timeout);
mp_int_t common_hal_usb_core_device_ctrl_transfer(usb_core_device_obj_t *self,
    mp_int_t bmRequestType, mp_int_t bRequest,
    mp_int_t wValue, mp_int_t wIndex,
    uint8_t *buffer, mp_int_t len, mp_int_t timeout);

bool common_hal_usb_core_device_is_kernel_driver_active(usb_core_device_obj_t *self, mp_int_t interface);
void common_hal_usb_core_device_detach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface);
void common_hal_usb_core_device_attach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_USB_CORE_DEVICE_H
