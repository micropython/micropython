/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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


// micropython includes
#include "lib/oofatfs/ff.h"
#include "py/runtime.h"


// MTB includes
#include "cyhal.h"


// object defined in main
extern cyhal_rtc_t psoc6_rtc;


MP_WEAK DWORD get_fattime(void) {
    struct tm current_date_time = {0};
    cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
    }

    return ((current_date_time.tm_year - 1980) << 25) | ((current_date_time.tm_mon) << 21) | ((current_date_time.tm_mday) << 16) | ((current_date_time.tm_hour) << 11) | ((current_date_time.tm_min) << 5) | (current_date_time.tm_sec / 2);
}
