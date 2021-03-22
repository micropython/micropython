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

#include "src/rp2040/hardware_regs/include/hardware/platform_defs.h"

#define MICROPY_PY_SYS_PLATFORM             "RP2040"

#define CIRCUITPY_INTERNAL_NVM_SIZE         (4 * 1024)
#define CIRCUITPY_INTERNAL_NVM_START_ADDR   (0x100FF000)

#define CIRCUITPY_DEFAULT_STACK_SIZE        (24 * 1024)

#define MICROPY_USE_INTERNAL_PRINTF         (1)

#define CIRCUITPY_PROCESSOR_COUNT           (2)

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    mp_obj_t counting[NUM_PWM_SLICES]; \
    mp_obj_t playing_audio[NUM_DMA_CHANNELS]; \
    CIRCUITPY_COMMON_ROOT_POINTERS;

#endif  // __INCLUDED_MPCONFIGPORT_H
