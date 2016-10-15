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

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_RNG

#include "py/obj.h"
#include "rng.h"

#include "kinetis.h"

// The following are not yet defined in kinetis.h
#define RNG_CR_GO_MASK          0x1u
#define RNG_CR_HA_MASK          0x2u
#define RNG_CR_INTM_MASK        0x4u
#define RNG_CR_CLRI_MASK        0x8u
#define RNG_CR_SLP_MASK         0x10u
#define RNG_SR_OREG_LVL_MASK    0xFF00u
#define RNG_SR_OREG_LVL_SHIFT   8
#define RNG_SR_OREG_LVL(x)      (((uint32_t)(((uint32_t)(x))<<RNG_SR_OREG_LVL_SHIFT))&RNG_SR_OREG_LVL_MASK)

/// \moduleref pyb

void rng_init(void) {
    SIM_SCGC6 |= SIM_SCGC6_RNGA; // enable RNG
    RNG_CR &= ~RNG_CR_SLP_MASK;
    RNG_CR |= RNG_CR_HA_MASK;  // high assurance, not needed
}

uint32_t rng_get(void) {
    if ((SIM_SCGC6 & SIM_SCGC6_RNGA) == 0) {
        rng_init();
    }
    RNG_CR |= RNG_CR_GO_MASK;
    while((RNG_SR & RNG_SR_OREG_LVL(0xF)) == 0); // wait
    return RNG_OR;
}

/// \function rng()
/// Return a 30-bit hardware generated random number.
STATIC mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(rng_get() >> 2);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);

#endif // MICROPY_HW_ENABLE_RNG
