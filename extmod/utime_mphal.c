/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "py/mpconfig.h"
#if MICROPY_PY_UTIME_MP_HAL

#include <string.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "py/smallint.h"
#include "extmod/utime_mphal.h"

STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_hal_delay_ms(1000 * mp_obj_get_float(seconds_o));
    #else
    mp_hal_delay_ms(1000 * mp_obj_get_int(seconds_o));
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_utime_sleep_obj, time_sleep);

STATIC mp_obj_t time_sleep_ms(mp_obj_t arg) {
    mp_hal_delay_ms(mp_obj_get_int(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_utime_sleep_ms_obj, time_sleep_ms);

STATIC mp_obj_t time_sleep_us(mp_obj_t arg) {
    mp_hal_delay_us(mp_obj_get_int(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_utime_sleep_us_obj, time_sleep_us);

STATIC mp_obj_t time_ticks_ms(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_ms() & MP_SMALL_INT_POSITIVE_MASK);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_utime_ticks_ms_obj, time_ticks_ms);

STATIC mp_obj_t time_ticks_us(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_us() & MP_SMALL_INT_POSITIVE_MASK);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_utime_ticks_us_obj, time_ticks_us);

STATIC mp_obj_t time_ticks_cpu(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_cpu() & MP_SMALL_INT_POSITIVE_MASK);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_utime_ticks_cpu_obj, time_ticks_cpu);

STATIC mp_obj_t time_ticks_diff(mp_obj_t start_in, mp_obj_t end_in) {
    // we assume that the arguments come from ticks_xx so are small ints
    uint32_t start = MP_OBJ_SMALL_INT_VALUE(start_in);
    uint32_t end = MP_OBJ_SMALL_INT_VALUE(end_in);
    return MP_OBJ_NEW_SMALL_INT((end - start) & MP_SMALL_INT_POSITIVE_MASK);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_utime_ticks_diff_obj, time_ticks_diff);

#endif // MICROPY_PY_UTIME_MP_HAL
