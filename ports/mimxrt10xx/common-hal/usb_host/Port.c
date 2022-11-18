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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/usb_host/Port.h"

#include "py/runtime.h"

bool usb_host_init;

void common_hal_usb_host_port_construct(usb_host_port_obj_t *self, const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    const mcu_pin_obj_t *supported_dp;
    const mcu_pin_obj_t *supported_dm;
    if (CIRCUITPY_USB_HOST_INSTANCE == 0) {
        supported_dp = &pin_USB_OTG1_DP;
        supported_dm = &pin_USB_OTG1_DN;
    } else if (CIRCUITPY_USB_HOST_INSTANCE == 1) {
        supported_dp = &pin_USB_OTG2_DP;
        supported_dm = &pin_USB_OTG2_DN;
    }
    if (dp != supported_dp || dm != supported_dm) {
        raise_ValueError_invalid_pins();
    }
    self->init = true;
    usb_host_init = true;
}

void common_hal_usb_host_port_deinit(usb_host_port_obj_t *self) {
    self->init = false;
    usb_host_init = false;
}

bool common_hal_usb_host_port_deinited(usb_host_port_obj_t *self) {
    return !self->init;
}
