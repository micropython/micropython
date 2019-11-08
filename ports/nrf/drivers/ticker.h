/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 The MicroPython-on-micro:bit Developers:
 * Damien P. George (@dpgeorge)
 * Nicholas H. Tollervey (@ntoll)
 * Matthew Else (@matthewelse)
 * Alan M. Jackson (@alanmjackson)
 * Mark Shannon (@markshannon)
 * Larry Hastings (@larryhastings)
 * Mariia Koroliuk (@marichkakorolyuk)
 * Andrew Mulholland (@gbaman)
 * Joe Glancy (@JoeGlancy)
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

#ifndef __MICROPY_INCLUDED_LIB_TICKER_H__
#define __MICROPY_INCLUDED_LIB_TICKER_H__

/*************************************
 * 62.5kHz (16us cycle time) ticker.
 ************************************/

#include "nrf.h"

typedef void (*callback_ptr)(void);
typedef int32_t (*ticker_callback_ptr)(void);

void ticker_init0(void);
void ticker_start(void);
void ticker_stop(void);
int clear_ticker_callback(uint32_t index);
int set_ticker_callback(uint32_t index, ticker_callback_ptr func, int32_t initial_delay_us);

void ticker_register_low_pri_callback(callback_ptr callback);

#define CYCLES_PER_MICROSECONDS 16

#define MICROSECONDS_PER_TICK 16
#define CYCLES_PER_TICK (CYCLES_PER_MICROSECONDS*MICROSECONDS_PER_TICK)
// This must be an integer multiple of MICROSECONDS_PER_TICK
#define MICROSECONDS_PER_MACRO_TICK 6000
#define MILLISECONDS_PER_MACRO_TICK 6

#endif // __MICROPY_INCLUDED_LIB_TICKER_H__
