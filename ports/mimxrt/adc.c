/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#include "py/runtime.h"
#include "py/mphal.h"

#include "adc.h"


void adc_init(void) {
    ADC_Type *const adcBases[] = ADC_BASE_PTRS;

    for (int i = 1; i < sizeof(adcBases) / sizeof(ADC_Type *); ++i)
    {
        ADC_Type *adc_instance = adcBases[i];

        // Configure ADC perpheral
        adc_config_t config;
        ADC_GetDefaultConfig(&config);
        ADC_Init(adc_instance, &config);

        // Perform calibration
        status_t calib_state = ADC_DoAutoCalibration(adc_instance);

        if (calib_state == kStatus_Fail) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Calibration for ADC Instance %d failed", i));
        }
    }
}