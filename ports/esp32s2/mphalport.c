/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>

#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/gc.h"

#include "components/xtensa/include/esp_debug_helpers.h"
#include "components/esp_rom/include/esp32s2/rom/ets_sys.h"

void mp_hal_delay_us(mp_uint_t delay) {
    ets_delay_us(delay);
}

// This is provided by the esp-idf/components/xtensa/esp32s2/libhal.a binary
// blob.
extern void xthal_window_spill(void);

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs, uint8_t reg_count) {
    // xtensa has more registers than an instruction can address. The 16 that
    // can be addressed are called the "window". When a function is called or
    // returns the window rotates. This allows for more efficient function calls
    // because ram doesn't need to be used. It's only used if the window wraps
    // around onto itself. At that point values are "spilled" to empty spots in
    // the stack that were set aside. When the window rotates back around (on
    // function return), the values are restored into the register from ram.

    // So, in order to read the values in the stack scan we must make sure all
    // of the register values we care about have been spilled to RAM. Luckily,
    // there is a HAL call to do it. There is a bit of a race condition here
    // because the register value could change after it's been restored but that
    // is unlikely to happen with a heap pointer while we do a GC.
    xthal_window_spill();
    return (mp_uint_t)__builtin_frame_address(0);
}
