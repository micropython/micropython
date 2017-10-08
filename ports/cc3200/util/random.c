/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include "py/obj.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pybrtc.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "random.h"
#include "debug.h"

/******************************************************************************
* LOCAL TYPES
******************************************************************************/
typedef union _rng_id_t {
    uint32_t       id32;
    uint16_t       id16[3];
    uint8_t        id8[6];
} rng_id_t;

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/
static uint32_t s_seed;

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/
STATIC uint32_t lfsr (uint32_t input);

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/
STATIC uint32_t lfsr (uint32_t input) {
    assert( input != 0 );
    return (input >> 1) ^ (-(input & 0x01) & 0x00E10000);
}

/******************************************************************************/
// MicroPython bindings;

STATIC mp_obj_t machine_rng_get(void) {
    return mp_obj_new_int(rng_get());
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_rng_get_obj, machine_rng_get);

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/
void rng_init0 (void) {
    rng_id_t juggler;
    uint32_t seconds;
    uint16_t mseconds;

    // get the seconds and the milliseconds from the RTC
    pyb_rtc_get_time(&seconds, &mseconds);

    wlan_get_mac (juggler.id8);

    // flatten the 48-bit board identification to 24 bits
    juggler.id16[0] ^= juggler.id16[2];

    juggler.id8[0]  ^= juggler.id8[3];
    juggler.id8[1]  ^= juggler.id8[4];
    juggler.id8[2]  ^= juggler.id8[5];

    s_seed = juggler.id32 & 0x00FFFFFF;
    s_seed += (seconds & 0x000FFFFF) + mseconds;

    // the seed must not be zero
    if (s_seed == 0) {
        s_seed = 1;
    }
}

uint32_t rng_get (void) {
    s_seed = lfsr( s_seed );
    return s_seed;
}
