/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <string.h>

#include STM32_HAL_H

#include "py/obj.h"
#include "rng.h"

#if MICROPY_HW_ENABLE_RNG

/// \moduleref pyb

STATIC RNG_HandleTypeDef RNGHandle = {.Instance = NULL};

void rng_init0(void) {
    // reset the RNG handle
    memset(&RNGHandle, 0, sizeof(RNG_HandleTypeDef));
    RNGHandle.Instance = RNG;
}

void rng_init(void) {
    __RNG_CLK_ENABLE();
    HAL_RNG_Init(&RNGHandle);
}

uint32_t rng_get(void) {
    if (RNGHandle.State == HAL_RNG_STATE_RESET) {
        rng_init();
    }
    return HAL_RNG_GetRandomNumber(&RNGHandle);
}

/// \function rng()
/// Return a 30-bit hardware generated random number.
STATIC mp_obj_t pyb_rng_get(void) {
    if (RNGHandle.State == HAL_RNG_STATE_RESET) {
        rng_init();
    }
    return mp_obj_new_int(HAL_RNG_GetRandomNumber(&RNGHandle) >> 2);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);

#endif // MICROPY_HW_ENABLE_RNG
