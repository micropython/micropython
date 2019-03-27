/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
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

#include "common-hal/microcontroller/Processor.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#endif

#include "nrf.h"

float common_hal_mcu_processor_get_temperature(void) {
    int32_t temp = 0;

#ifdef BLUETOOTH_SD
    uint8_t sd_en = 0;

    (void) sd_softdevice_is_enabled(&sd_en);

    if (sd_en) {
        uint32_t err_code = sd_temp_get(&temp);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg(translate("Cannot get temperature"));
        }
    }
#endif

    NRF_TEMP->TASKS_START = 1;

    while (NRF_TEMP->EVENTS_DATARDY == 0)
        ;

    NRF_TEMP->EVENTS_DATARDY = 0;

    temp = NRF_TEMP->TEMP;

    NRF_TEMP->TASKS_STOP = 1;

    return temp / 4.0f;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return 64000000ul;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    for (int i=0; i<2; i++) {
        ((uint32_t*) raw_id)[i] = NRF_FICR->DEVICEID[i];
    }
}
