/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FOURWIRE_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FOURWIRE_H

#include "common-hal/busio/SPI.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/displayio/Group.h"

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *bus;
    busio_spi_obj_t inline_bus;
    digitalio_digitalinout_obj_t command;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t reset;
    uint32_t frequency;
    uint8_t polarity;
    uint8_t phase;
} displayio_fourwire_obj_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FOURWIRE_H
