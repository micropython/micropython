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

#ifndef FPGA_MPCONFIGPORT_H__
#define FPGA_MPCONFIGPORT_H__

#define CIRCUITPY_INTERNAL_NVM_SIZE         (0)
#define MICROPY_NLR_THUMB                   (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS  (1)

#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS
#define MICROPY_NLR_SETJMP                  (1)
#define CIRCUITPY_DEFAULT_STACK_SIZE        0x6000


#endif  // __INCLUDED_FPGA_MPCONFIGPORT_H
