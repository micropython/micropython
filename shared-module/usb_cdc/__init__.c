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

#include "genhdr/autogen_usb_descriptor.h"
#include "py/gc.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_cdc/__init__.h"
#include "shared-bindings/usb_cdc/Serial.h"
#include "tusb.h"

#if CFG_TUD_CDC != 2
#error CFG_TUD_CDC must be exactly 2
#endif

static usb_cdc_serial_obj_t serial_objs[CFG_TUD_CDC] = {
    {   .base.type = &usb_cdc_serial_type,
        .timeout = -1.0f,
        .idx = 0,
    }, {
        .base.type = &usb_cdc_serial_type,
        .timeout = -1.0f,
        .idx = 1,
    }
};

const mp_rom_obj_tuple_t usb_cdc_serials_tuple = {
    .base.type = &mp_type_tuple,
    .len = CFG_TUD_CDC,
    .items = {
        &serial_objs[0],
        &serial_objs[1],
    },
};
