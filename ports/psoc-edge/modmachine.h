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

#ifndef MICROPY_INCLUDED_PSOCEDGE_MODMACHINE_H
#define MICROPY_INCLUDED_PSOCEDGE_MODMACHINE_H


// micropython includes
#include "py/obj.h"

enum clock_freq_type {
    AUDIO_SYS_CLOCK_73_728_000_HZ = 73728000UL /* (Ideally 73.728 MHz) For sample rates: 8 KHz / 16 KHz / 48 KHz */,
    AUDIO_SYS_CLOCK_169_344_000_HZ = 169344000UL /* (Ideally 169.344 MHz) For sample rates: 22.05 KHz / 44.1 KHz */,
    CM4,
    CM4_FLL
};

extern enum clock_freq_type PLL0_freq;

#endif // MICROPY_INCLUDED_PSOCEDGE_MODMACHINE_H
