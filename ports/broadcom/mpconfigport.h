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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

// Definitions that control circuitpy_mpconfig.h:

////////////////////////////////////////////////////////////////////////////////////////////////////


#define CIRCUITPY_MCU_FAMILY                        broadcom
#define MICROPY_PY_SYS_PLATFORM                     "BROADCOM"
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (1)
#define MICROPY_PY_FUNCTION_ATTRS                   (1)
#if BCM_VERSION == 2837 || BCM_VERSION == 2711
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_A)
#elif BCM_VERSION == 2835
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#endif
#define CIRCUITPY_DEFAULT_STACK_SIZE            (0x10000)
#define CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE (1920)
#define CIRCUITPY_PROCESSOR_COUNT (4)

#define MICROPY_FATFS_EXFAT    (1)
#define MICROPY_FATFS_MKFS_FAT32  (1)

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

// Definitions that can be overridden by mpconfigboard.h:

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  // __INCLUDED_MPCONFIGPORT_H
