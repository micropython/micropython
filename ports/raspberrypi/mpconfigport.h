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

// Setting a non-default value also requires a non-default link.ld
#ifndef CIRCUITPY_FIRMWARE_SIZE
#define CIRCUITPY_FIRMWARE_SIZE (1020 * 1024)
#endif

#define CIRCUITPY_INTERNAL_NVM_SIZE         (4 * 1024)
// This is the XIP address
#define CIRCUITPY_INTERNAL_NVM_START_ADDR   (0x10000000 + CIRCUITPY_FIRMWARE_SIZE)

// This is the flash linear address
#define CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR (CIRCUITPY_FIRMWARE_SIZE + CIRCUITPY_INTERNAL_NVM_SIZE)
#define CIRCUITPY_DEFAULT_STACK_SIZE        (24 * 1024)

#define MICROPY_USE_INTERNAL_PRINTF         (1)

#define CIRCUITPY_PROCESSOR_COUNT           (2)

#if CIRCUITPY_USB_HOST
#define CIRCUITPY_USB_HOST_INSTANCE 1
#endif

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

#if CIRCUITPY_CYW43
#define MICROPY_PY_LWIP_ENTER   cyw43_arch_lwip_begin();
#define MICROPY_PY_LWIP_REENTER MICROPY_PY_LWIP_ENTER
#define MICROPY_PY_LWIP_EXIT    cyw43_arch_lwip_end();
#endif

// Protect the background queue with a lock because both cores may modify it.
#include "pico/critical_section.h"
extern critical_section_t background_queue_lock;
#define CALLBACK_CRITICAL_BEGIN (critical_section_enter_blocking(&background_queue_lock))
#define CALLBACK_CRITICAL_END (critical_section_exit(&background_queue_lock))

// Turn some macros into compile-time constants, using enum.
// Some nested macros expand across multiple lines, which is not
// handled by the MP_REGISTER_ROOT_POINTER processing in makeqstrdefs.py.
enum {
    enum_NUM_DMA_CHANNELS = NUM_DMA_CHANNELS,
    enum_NUM_PWM_SLICES = NUM_PWM_SLICES,
};

#endif  // __INCLUDED_MPCONFIGPORT_H
