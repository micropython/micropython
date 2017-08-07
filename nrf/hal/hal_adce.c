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
#include "hal_adc.h"

#ifdef HAL_ADCE_MODULE_ENABLED

static const uint32_t hal_adc_input_lookup_pos[] = {
    SAADC_CH_PSELP_PSELP_AnalogInput0 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput1 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput2 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput3 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput4 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput5 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput6 << SAADC_CH_PSELP_PSELP_Pos,
    SAADC_CH_PSELP_PSELP_AnalogInput7 << SAADC_CH_PSELP_PSELP_Pos
};

#define HAL_ADCE_PSELP_NOT_CONNECTED (SAADC_CH_PSELP_PSELP_NC << SAADC_CH_PSELP_PSELP_Pos)
#define HAL_ADCE_PSELP_VDD           (SAADC_CH_PSELP_PSELP_VDD << SAADC_CH_PSELP_PSELP_Pos)

/*static const uint32_t hal_adc_input_lookup_neg[] = {
    SAADC_CH_PSELN_PSELN_AnalogInput0 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput1 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput2 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput3 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput4 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput5 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput6 << SAADC_CH_PSELN_PSELN_Pos,
    SAADC_CH_PSELN_PSELN_AnalogInput7 << SAADC_CH_PSELN_PSELN_Pos
};*/

#define HAL_ADCE_PSELN_NOT_CONNECTED (SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos)
#define HAL_ADCE_PSELN_VDD           (SAADC_CH_PSELN_PSELN_VDD << SAADC_CH_PSELN_PSELN_Pos)

uint16_t hal_adc_channel_value(hal_adc_config_t const * p_adc_conf) {
    int16_t result = 0;

    // configure to use VDD/4 and gain 1/4
    ADC_BASE->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_4  << SAADC_CH_CONFIG_GAIN_Pos)
                           | (SAADC_CH_CONFIG_MODE_SE       << SAADC_CH_CONFIG_MODE_Pos)
                           | (SAADC_CH_CONFIG_REFSEL_VDD1_4 << SAADC_CH_CONFIG_REFSEL_Pos)
                           | (SAADC_CH_CONFIG_RESN_Bypass   << SAADC_CH_CONFIG_RESN_Pos)
                           | (SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESP_Pos)
                           | (SAADC_CH_CONFIG_TACQ_3us      << SAADC_CH_CONFIG_TACQ_Pos);

    // positive input
    ADC_BASE->CH[0].PSELP = hal_adc_input_lookup_pos[p_adc_conf->channel]; // HAL_ADCE_PSELP_VDD;
    ADC_BASE->CH[0].PSELN = HAL_ADCE_PSELN_NOT_CONNECTED;

    ADC_BASE->RESOLUTION    = SAADC_RESOLUTION_VAL_8bit << SAADC_RESOLUTION_VAL_Pos;
    ADC_BASE->RESULT.MAXCNT = 1;
    ADC_BASE->RESULT.PTR    = (uint32_t)&result;
    ADC_BASE->SAMPLERATE    = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;
    ADC_BASE->ENABLE        = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;

    // calibrate ADC
    ADC_BASE->TASKS_CALIBRATEOFFSET = 1;
    while (ADC_BASE->EVENTS_CALIBRATEDONE == 0) {
        ;
    }
    ADC_BASE->EVENTS_CALIBRATEDONE = 0;
    while (ADC_BASE->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos)) {
        ;
    }

    // start the ADC
    ADC_BASE->TASKS_START = 1;
    while (ADC_BASE->EVENTS_STARTED == 0) {
        ;
    }
    ADC_BASE->EVENTS_STARTED = 0;

    // sample ADC
    ADC_BASE->TASKS_SAMPLE = 1;
    while (ADC_BASE->EVENTS_END == 0) {
        ;
    }
    ADC_BASE->EVENTS_END = 0;

    ADC_BASE->TASKS_STOP = 1;
    while (ADC_BASE->EVENTS_STOPPED == 0) {
        ;
    }
    ADC_BASE->EVENTS_STOPPED = 0;

    return result;
}

uint16_t hal_adc_battery_level(void) {
    return 0;
}

#endif // HAL_ADCE_MODULE_ENABLED
