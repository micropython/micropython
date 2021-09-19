/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef ESP32S2_MPCONFIGPORT_H__
#define ESP32S2_MPCONFIGPORT_H__

#define MICROPY_NLR_THUMB                   (0)

#define MICROPY_USE_INTERNAL_PRINTF         (0)
#define MICROPY_PY_SYS_PLATFORM             "Espressif ESP32-S2"

#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS
#define MICROPY_NLR_SETJMP                  (1)
#define CIRCUITPY_DEFAULT_STACK_SIZE        0x6000

#define CIRCUITPY_INTERNAL_NVM_START_ADDR (0x9000)

// 20kB is statically allocated to nvs, but when overwriting an existing
// item, it's temporarily necessary to store both the old and new copies.
// Additionally, there is some overhad for the names and values of items
// in nvs, and alignment to 4kB flash erase boundaries may give better
// performance characteristics (h/t @tannewt). This implies we should select an
// 8kB size for CircuitPython'ns NVM.
#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE (8 * 1024)
#endif

// Define to (1) in mpconfigboard.h if the board has a defined I2C port that
// lacks pull up resistors (Espressif's HMI Devkit), and the internal pull-up
// resistors will be enabled for all busio.I2C objects. This is only to
// compensate for design decisions that are out of the control of the authors
// of CircuitPython and is not an endorsement of running without appropriate
// external pull up resistors.
#ifndef CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP
#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (0)
#endif
#endif  // __INCLUDED_ESP32S2_MPCONFIGPORT_H
