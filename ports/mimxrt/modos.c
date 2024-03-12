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

#if defined(MIMXRT117x_SERIES)
#include "fsl_caam.h"
#else
#include "fsl_trng.h"
#endif

static bool initialized = false;

#if defined(MIMXRT117x_SERIES)
static caam_handle_t caam_handle;
static caam_rng_state_handle_t caam_state_handle = kCAAM_RngStateHandle0;

#if defined(FSL_FEATURE_HAS_L1CACHE) || defined(__DCACHE_PRESENT)
AT_NONCACHEABLE_SECTION(static caam_job_ring_interface_t s_jrif0);
#else
static caam_job_ring_interface_t s_jrif0;
#endif

static void trng_start(void) {
    caam_config_t config;

    if (!initialized) {
        CAAM_GetDefaultConfig(&config);
        config.jobRingInterface[0] = &s_jrif0;
        CAAM_Init(CAAM, &config);
        initialized = true;
    }
}

void trng_random_data(unsigned char *output, size_t len) {
    trng_start();
    CAAM_RNG_GetRandomData(CAAM, &caam_handle, caam_state_handle, output, len,
        kCAAM_RngDataAny, NULL);
}

#else

static void trng_start(void) {
    trng_config_t trngConfig;

    if (!initialized) {
        TRNG_GetDefaultConfig(&trngConfig);
        trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;

        TRNG_Init(TRNG, &trngConfig);
        initialized = true;
    }
}

void trng_random_data(unsigned char *output, size_t len) {
    trng_start();
    TRNG_GetRandomData(TRNG, output, len);
}

#endif

uint32_t trng_random_u32(void) {
    uint32_t rngval;

    trng_start();
    trng_random_data((uint8_t *)&rngval, 4);
    return rngval;
}

#if MICROPY_PY_OS_URANDOM
static mp_obj_t mp_os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);

    trng_start();
    trng_random_data((uint8_t *)vstr.buf, n);

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_urandom_obj, mp_os_urandom);
#endif
