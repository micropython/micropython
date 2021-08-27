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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H

#include "py/obj.h"
#include "py/enum.h"

typedef enum {
    RESET_REASON_POWER_ON,
    RESET_REASON_BROWNOUT,
    RESET_REASON_SOFTWARE,
    RESET_REASON_DEEP_SLEEP_ALARM,
    RESET_REASON_RESET_PIN,
    RESET_REASON_WATCHDOG,
    RESET_REASON_UNKNOWN,
} mcu_reset_reason_t;

extern const mp_obj_type_t mcu_reset_reason_type;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H
