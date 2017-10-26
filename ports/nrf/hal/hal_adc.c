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

#include <stdio.h>
#include "mphalport.h"
#include "hal_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS  (1200) // Reference voltage (in milli volts) used by ADC while doing conversion.
#define ADC_PRE_SCALING_COMPENSATION   (3)    // The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS (270)  // Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com.

#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION)

static const uint32_t hal_adc_input_lookup[] = {
    ADC_CONFIG_PSEL_AnalogInput0 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput1 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput2 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput3 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput4 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput5 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput6 << ADC_CONFIG_PSEL_Pos,
    ADC_CONFIG_PSEL_AnalogInput7 << ADC_CONFIG_PSEL_Pos
};


static uint8_t battery_level_in_percent(const uint16_t mvolts)
{
    uint8_t battery_level;

    if (mvolts >= 3000) {
        battery_level = 100;
    } else if (mvolts > 2900) {
        battery_level = 100 - ((3000 - mvolts) * 58) / 100;
    } else if (mvolts > 2740) {
        battery_level = 42 - ((2900 - mvolts) * 24) / 160;
    } else if (mvolts > 2440) {
        battery_level = 18 - ((2740 - mvolts) * 12) / 300;
    } else if (mvolts > 2100) {
        battery_level = 6 - ((2440 - mvolts) * 6) / 340;
    } else {
        battery_level = 0;
    }

    return battery_level;
}

uint16_t hal_adc_channel_value(hal_adc_config_t const * p_adc_conf) {
    ADC_BASE->INTENSET   = ADC_INTENSET_END_Msk;
    ADC_BASE->CONFIG     = (ADC_CONFIG_RES_8bit                        << ADC_CONFIG_RES_Pos)
                           | (ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling << ADC_CONFIG_INPSEL_Pos)
                           | (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)
                           | (hal_adc_input_lookup[p_adc_conf->channel])
                           | (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);

    ADC_BASE->EVENTS_END  = 0;
    ADC_BASE->ENABLE      = ADC_ENABLE_ENABLE_Enabled;

    ADC_BASE->EVENTS_END  = 0;
    ADC_BASE->TASKS_START = 1;

    while (!ADC_BASE->EVENTS_END) {
        ;
    }

    uint8_t  adc_result;

    ADC_BASE->EVENTS_END = 0;
    adc_result           = ADC_BASE->RESULT;
    ADC_BASE->TASKS_STOP = 1;

    return adc_result;
}

uint16_t hal_adc_battery_level(void) {
    ADC_BASE->INTENSET   = ADC_INTENSET_END_Msk;
    ADC_BASE->CONFIG     = (ADC_CONFIG_RES_8bit                        << ADC_CONFIG_RES_Pos)
                           | (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)
                           | (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)
                           | (ADC_CONFIG_PSEL_Disabled                   << ADC_CONFIG_PSEL_Pos)
                           | (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);

    ADC_BASE->EVENTS_END  = 0;
    ADC_BASE->ENABLE      = ADC_ENABLE_ENABLE_Enabled;

    ADC_BASE->EVENTS_END  = 0;
    ADC_BASE->TASKS_START = 1;

    while (!ADC_BASE->EVENTS_END) {
        ;
    }

    uint8_t  adc_result;
    uint16_t batt_lvl_in_milli_volts;

    ADC_BASE->EVENTS_END = 0;
    adc_result           = ADC_BASE->RESULT;
    ADC_BASE->TASKS_STOP = 1;

    batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result) + DIODE_FWD_VOLT_DROP_MILLIVOLTS;
    return battery_level_in_percent(batt_lvl_in_milli_volts);
}

#endif // HAL_ADC_MODULE_ENABLED
