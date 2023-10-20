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

#include "tusb.h"

#include "imx_usb.h"

usb_host_port_obj_t usb_host_instance;

usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    const mcu_pin_obj_t *supported_dp;
    const mcu_pin_obj_t *supported_dm;
    if (CIRCUITPY_USB_HOST_INSTANCE == 0) {
        supported_dp = &pin_USB_OTG1_DP;
        supported_dm = &pin_USB_OTG1_DN;
    } else if (CIRCUITPY_USB_HOST_INSTANCE == 1) {
        supported_dp = &pin_USB_OTG2_DP;
        supported_dm = &pin_USB_OTG2_DN;
    }
    // Return the singleton if given the same pins.
    usb_host_port_obj_t *self = &usb_host_instance;
    if (self->dp != NULL) {
        if (self->dp != dp || self->dm != dm) {
            mp_raise_msg_varg(&mp_type_RuntimeError, translate("%q in use"), MP_QSTR_usb_host);
        }
        return self;
    }
    if (dp != supported_dp || dm != supported_dm) {
        raise_ValueError_invalid_pins();
    }

    assert_pin_free(dp);
    assert_pin_free(dm);

    init_usb_instance(CIRCUITPY_USB_HOST_INSTANCE);
    tuh_init(TUH_OPT_RHPORT);

    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;

    return self;
}
