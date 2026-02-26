/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <stdlib.h>

// mpy includes
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modnetwork.h"

// port-specific includes
#include "cybsp.h"
#include "modmachine.h"
#include "mplogger.h"
#include "modpsocedge.h"
#if MICROPY_PY_MACHINE

enum clock_freq_type PLL0_freq = AUDIO_SYS_CLOCK_73_728_000_HZ;
enum clock_freq_type freq_peri;

// machine.idle()
// This executies a wfi machine instruction which reduces power consumption
// of the MCU until an interrupt occurs, at which point execution continues.
static void mp_machine_idle(void) {
    __WFI(); // standard ARM instruction
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    freq_peri = mp_obj_get_int(args[0]); // Assuming the enum values are used as integers
    switch (freq_peri) {
        case AUDIO_SYS_CLOCK_73_728_000_HZ:
            PLL0_freq = AUDIO_SYS_CLOCK_73_728_000_HZ;
            break;
        case AUDIO_SYS_CLOCK_169_344_000_HZ:
            PLL0_freq = AUDIO_SYS_CLOCK_169_344_000_HZ;
            break;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid frequency type %lu"), freq_peri);
            break;
    }
}

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    /* Modules */ \

#endif // MICROPY_PY_MACHINE
