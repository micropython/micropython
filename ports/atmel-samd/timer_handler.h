/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_TIMER_HANDLER_H
#define MICROPY_INCLUDED_ATMEL_SAMD_TIMER_HANDLER_H

#define TC_HANDLER_NO_INTERRUPT 0x0
#define TC_HANDLER_PULSEOUT 0x1
#define TC_HANDLER_PEW 0x2
#define TC_HANDLER_FREQUENCYIN 0x3
#define TC_HANDLER_RGBMATRIX 0x4
#define TC_HANDLER_PULSEIN 0x5

void set_timer_handler(bool is_tc, uint8_t index, uint8_t timer_handler);
void shared_timer_handler(bool is_tc, uint8_t index);

// implementation of these functions is in PWMOut.c
void timer_never_reset(int index, bool is_tc);
void timer_reset_ok(int index, bool is_tc);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_TIMER_HANDLER_H
