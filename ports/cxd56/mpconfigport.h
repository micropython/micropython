/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#define MICROPY_PY_SYS_PLATFORM                 "CXD56"

// 64kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            (0x10000)

// CXD56 architecture uses fixed endpoint numbers.
// Override default definitions in circuitpy_mpconfig.h,
// so define these before #include'ing that file.
#define USB_CDC_EP_NUM_NOTIFICATION (3)
#define USB_CDC_EP_NUM_DATA_OUT (2)
#define USB_CDC_EP_NUM_DATA_IN (1)
#define USB_MSC_EP_NUM_OUT (5)
#define USB_MSC_EP_NUM_IN (4)

#include "py/circuitpy_mpconfig.h"

#define MICROPY_BYTES_PER_GC_BLOCK              (32)

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \

#endif  // __INCLUDED_MPCONFIGPORT_H
