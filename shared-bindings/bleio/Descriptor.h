/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H

#include "common-hal/bleio/Descriptor.h"
#include "common-hal/bleio/UUID.h"

enum {
    DESCRIPTOR_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES = 0x2900,
    DESCRIPTOR_UUID_CHARACTERISTIC_USER_DESCRIPTION = 0x2901,
    DESCRIPTOR_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION = 0x2902,
    DESCRIPTOR_UUID_SERVER_CHARACTERISTIC_CONFIGURATION = 0x2903,
    DESCRIPTOR_UUID_CHARACTERISTIC_PRESENTATION_FORMAT = 0x2904,
    DESCRIPTOR_UUID_CHARACTERISTIC_AGGREGATE_FORMAT = 0x2905,
    DESCRIPTOR_UUID_VALID_RANGE = 0x2906,
    DESCRIPTOR_UUID_EXTERNAL_REPORT_REFERENCE = 0x2907,
    DESCRIPTOR_UUID_REPORT_REFERENCE = 0x2908,
    DESCRIPTOR_UUID_NUMBER_OF_DIGITALS = 0x2909,
    DESCRIPTOR_UUID_VALUE_TRIGGER_SETTING = 0x290A,
    DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_CONFIGURATION = 0x290B,
    DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_MEASUREMENT = 0x290C,
    DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_TRIGGER_SETTING = 0x290D,
    DESCRIPTOR_UUID_TIME_TRIGGER_SETTING = 0x290E,
};

extern const mp_obj_type_t bleio_descriptor_type;

extern void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self, bleio_uuid_obj_t *uuid);
extern mp_int_t common_hal_bleio_descriptor_get_handle(bleio_descriptor_obj_t *self);
extern mp_obj_t common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H
