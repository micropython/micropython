/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/board.h"

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/usb_host/Port.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

usb_host_port_obj_t _host_port;
digitalio_digitalinout_obj_t _host_power;

void board_init(void) {
    common_hal_digitalio_digitalinout_construct(&_host_power, &pin_GPIO18);
    common_hal_digitalio_digitalinout_never_reset(&_host_power);
    common_hal_digitalio_digitalinout_switch_to_output(&_host_power, true, DRIVE_MODE_PUSH_PULL);

    common_hal_usb_host_port_construct(&_host_port, &pin_GPIO16, &pin_GPIO17);
}
