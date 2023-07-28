/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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

#include <stdint.h>
#include <stdbool.h>

#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_HW_ENABLE_RNG

#include "rng.h"
#if defined(RA6M3)
#include "hw_sce_private.h"
#include "hw_sce_trng_private.h"
#elif defined(RA6M5)
#include "r_sce.h"
#endif

uint32_t rng_read(void) {
    uint32_t random_data[4];
    static bool initialized = false;

    if (initialized == false) {
        initialized = true;
        #if defined(RA6M3)
        HW_SCE_McuSpecificInit();
    }
    HW_SCE_RNG_Read(random_data);
        #elif defined(RA6M5)
        R_SCE_Open(&sce_ctrl, &sce_cfg);
    }
    R_SCE_RandomNumberGenerate(random_data);
    #endif

    return random_data[0];
}

#endif
