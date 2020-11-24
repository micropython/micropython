/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include <stdbool.h>
#include "py/mpconfig.h"
#include "tusb.h"

STATIC bool _allow_deep_sleep_when_connected;
STATIC bool _allow_deep_sleep_on_error;


void supervisor_workflow_reset(void) {
    _allow_deep_sleep_when_connected = false;
    _allow_deep_sleep_on_error = false;
}

bool supervisor_workflow_active(void) {
    // Eventually there might be other non-USB workflows, such as BLE.
    // tud_ready() checks for usb mounted and not suspended.
    return tud_ready();
}

bool supervisor_workflow_get_allow_deep_sleep_when_connected(void) {
    return _allow_deep_sleep_when_connected;
}

void supervisor_workflow_set_allow_deep_sleep_when_connected(bool allow) {
    _allow_deep_sleep_when_connected = allow;
}

bool supervisor_workflow_get_allow_deep_sleep_on_error(void) {
    return _allow_deep_sleep_on_error;
}

void supervisor_workflow_set_allow_deep_sleep_on_error(bool allow) {
    _allow_deep_sleep_on_error = allow;
}
