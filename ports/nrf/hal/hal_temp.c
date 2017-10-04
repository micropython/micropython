/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Bander F. Ajba
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

#include <stdio.h>
#include <stdint.h>
#include "mphalport.h"
#include "hal_temp.h"

#if BLUETOOTH_SD
#include "py/nlr.h"
#include "ble_drv.h"
#include "nrf_soc.h"
#define BLUETOOTH_STACK_ENABLED() (ble_drv_stack_enabled())
#endif // BLUETOOTH_SD

#ifdef HAL_TEMP_MODULE_ENABLED

void hal_temp_init(void) {
    // @note Workaround for PAN_028 rev2.0A anomaly 31 - TEMP: Temperature offset value has to be manually loaded to the TEMP module
    *(uint32_t *) 0x4000C504 = 0;
}



int32_t hal_temp_read(void) {
#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        int32_t temp;
        (void)sd_temp_get(&temp);
        return temp / 4; // resolution of 0.25 degree celsius
    }
#endif // BLUETOOTH_SD

    int32_t volatile temp;
    hal_temp_init();

    NRF_TEMP->TASKS_START = 1; // Start the temperature measurement.

    while (NRF_TEMP->EVENTS_DATARDY == 0) {
        // Do nothing.
    }

    NRF_TEMP->EVENTS_DATARDY = 0;

    // @note Workaround for PAN_028 rev2.0A anomaly 29 - TEMP: Stop task clears the TEMP register. 
    temp = (((NRF_TEMP->TEMP & MASK_SIGN) != 0) ? (NRF_TEMP->TEMP | MASK_SIGN_EXTENSION) : (NRF_TEMP->TEMP) / 4);

    // @note Workaround for PAN_028 rev2.0A anomaly 30 - TEMP: Temp module analog front end does not power down when DATARDY event occurs.
    NRF_TEMP->TASKS_STOP = 1; // Stop the temperature measurement. 
    return temp;
}

#endif // HAL_TEMP_MODULE_ENABLED
