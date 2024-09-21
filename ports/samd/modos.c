/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * use of the TRNG by
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
 * Copyright (c) 2022 Robert Hammelrath
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

// This file is never compiled standalone, it's included directly from
// extmod/modos.c via MICROPY_PY_OS_INCLUDEFILE.

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "sam.h"

#if defined(MCU_SAMD51)
static bool initialized = false;

static void trng_start(void) {
    if (!initialized) {
        MCLK->APBCMASK.bit.TRNG_ = 1;
        REG_TRNG_CTRLA = TRNG_CTRLA_ENABLE;
        initialized = true;
    }
}

uint32_t trng_random_u32(void) {
    trng_start();
    while ((REG_TRNG_INTFLAG & TRNG_INTFLAG_DATARDY) == 0) {
    }
    return REG_TRNG_DATA;
}

#define TRNG_RANDOM_U32 trng_random_u32()
#endif // defined(MCU_SAMD51)

#if defined(MCU_SAMD21)
extern volatile uint32_t rng_state;

uint32_t trng_random_u32(int delay) {
    mp_hal_delay_ms(delay); // wait a few cycles
    uint32_t upper = rng_state;
    mp_hal_delay_ms(delay); // wait again a few cycles
    return (upper & 0xffff0000) | (rng_state >> 16);
}

#define trng_start()
#define TRNG_RANDOM_U32 trng_random_u32(10)
#endif

#if MICROPY_PY_OS_URANDOM
static mp_obj_t mp_os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    uint32_t rngval = 0;

    trng_start();
    for (int i = 0; i < n; i++) {
        if ((i % 4) == 0) {
            rngval = TRNG_RANDOM_U32;
        }
        vstr.buf[i] = rngval & 0xff;
        rngval >>= 8;
    }
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_urandom_obj, mp_os_urandom);

#endif // MICROPY_PY_OS_URANDOM

#if MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM
bool mp_os_dupterm_is_builtin_stream(mp_const_obj_t stream) {
    const mp_obj_type_t *type = mp_obj_get_type(stream);
    return type == &machine_uart_type;
}
#endif
