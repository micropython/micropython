/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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
#include "hal_rng.h"

#ifdef HAL_RNG_MODULE_ENABLED

#if BLUETOOTH_SD
#include "py/nlr.h"
#include "ble_drv.h"
#include "nrf_soc.h"

#define BLUETOOTH_STACK_ENABLED() (ble_drv_stack_enabled())

#endif // BLUETOOTH_SD

uint32_t hal_rng_generate(void) {

    uint32_t retval = 0;

#if BLUETOOTH_SD

    if (BLUETOOTH_STACK_ENABLED() == 1) {
        uint32_t status;
        do {
            status = sd_rand_application_vector_get((uint8_t *)&retval, 4); // Extract 4 bytes
        } while (status != 0);
    } else {
#endif
        uint8_t * p_retval = (uint8_t *)&retval;

        NRF_RNG->EVENTS_VALRDY = 0;
        NRF_RNG->TASKS_START   = 1;

        for (uint16_t i = 0; i < 4; i++) {
            while (NRF_RNG->EVENTS_VALRDY == 0) {
                ;
            }
            NRF_RNG->EVENTS_VALRDY = 0;
            p_retval[i] = NRF_RNG->VALUE;
        }

        NRF_RNG->TASKS_STOP = 1;
#if BLUETOOTH_SD
    }
#endif

    return retval;
}

#endif // HAL_RNG_MODULE_ENABLED

