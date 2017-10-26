/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include "mphalport.h"
#include "hal_time.h"

#ifdef HAL_TIME_MODULE_ENABLED

void mp_hal_delay_us(mp_uint_t us)
{
    register uint32_t delay __ASM ("r0") = us;
    __ASM volatile (
#ifdef NRF51
            ".syntax unified\n"
#endif
        "1:\n"
        " SUBS %0, %0, #1\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
#ifdef NRF52
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
#endif
        " BNE 1b\n"
#ifdef NRF51
        ".syntax divided\n"
#endif
        : "+r" (delay));
}

void mp_hal_delay_ms(mp_uint_t ms)
{
    for (mp_uint_t i = 0; i < ms; i++)
    {
        mp_hal_delay_us(999);
    }
}

#endif // HAL_TIME_MODULE_ENABLED
