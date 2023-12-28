/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_USB_HOST_PORT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_USB_HOST_PORT_H

#include "py/objarray.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "common-hal/usb_host/Port.h"

extern const mp_obj_type_t usb_host_port_type;

// This is unique to common_hal constructs because it returns a globally stored
// object instead of taking on in that may be on the heap. This allows the
// method to check the internals of the global object against the given arguments
// to determine whether to return the singleton or raise an exception.
usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_USB_HOST_PORT_H
