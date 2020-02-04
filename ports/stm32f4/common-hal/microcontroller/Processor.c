/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include <math.h>
#include "common-hal/microcontroller/Processor.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "stm32f4xx_hal.h"

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

//Factory calibration locations
#define ADC_CAL_ADDRESS         (0x1fff7a2a)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 4))

// correction factor for reference value
STATIC volatile float adc_refcor = 1.0f;

#define CORE_TEMP_V25          (943)  /* (0.76v/3.3v)*(2^ADC resoultion) */
#define CORE_TEMP_AVG_SLOPE    (3)    /* (2.5mv/3.3v)*(2^ADC resoultion) */

float common_hal_mcu_processor_get_temperature(void) {
    __HAL_RCC_ADC1_CLK_ENABLE();

    //HAL Implementation
    ADC_HandleTypeDef AdcHandle;
    ADC_ChannelConfTypeDef sConfig;

    AdcHandle.Instance = ADC1;
    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
    AdcHandle.Init.ScanConvMode = DISABLE;
    AdcHandle.Init.ContinuousConvMode = DISABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 0;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.NbrOfConversion = 1;
    AdcHandle.Init.DMAContinuousRequests = DISABLE;
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&AdcHandle);

    ADC->CCR |= ADC_CCR_TSVREFE;
    ADC->CCR &= ~ADC_CCR_VBATE; // If this somehow got turned on, it'll return bad values. 

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR; //either 16 or 18, depending on chip
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; //Taken from micropython
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADC_Start(&AdcHandle);
    if (HAL_ADC_PollForConversion(&AdcHandle,1) != HAL_OK) {
        mp_raise_RuntimeError(translate("Temperature read timed out"));
    }
    uint32_t value = (uint32_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    //There's no F4 specific appnote for this but it works the same as the L1 in AN3964
    float core_temp_avg_slope = (*ADC_CAL2 - *ADC_CAL1) / 80.0;
    return (((float)value * adc_refcor - *ADC_CAL1) / core_temp_avg_slope) + 30.0f;

    // STATIC uint32_t adc_config_and_read_channel(ADC_HandleTypeDef *adcHandle, uint32_t channel) {
    // adc_config_channel(adcHandle, channel);
    // uint32_t raw_value = adc_read_channel(adcHandle);

    // #if defined(STM32F4) || defined(STM32F7)
    // // ST docs say that (at least on STM32F42x and STM32F43x), VBATE must
    // // be disabled when TSVREFE is enabled for TEMPSENSOR and VREFINT
    // // conversions to work.  VBATE is enabled by the above call to read
    // // the channel, and here we disable VBATE so a subsequent call for
    // // TEMPSENSOR or VREFINT works correctly.
    // if (channel == ADC_CHANNEL_VBAT) {
    //     ADC->CCR &= ~ADC_CCR_VBATE;
    // }
    // #endif

    // return raw_value;

    //     int adc_read_core_temp(ADC_HandleTypeDef *adcHandle) {
    //     int32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_TEMPSENSOR);
    //     return ((raw_value - CORE_TEMP_V25) / CORE_TEMP_AVG_SLOPE) + 25;
    // }

    // #if MICROPY_PY_BUILTINS_FLOAT
    // // correction factor for reference value
    // STATIC volatile float adc_refcor = 1.0f;

    // float adc_read_core_temp_float(ADC_HandleTypeDef *adcHandle) {
    //     int32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_TEMPSENSOR);
    //     float core_temp_avg_slope = (*ADC_CAL2 - *ADC_CAL1) / 80.0;
    //     return (((float)raw_value * adc_refcor - *ADC_CAL1) / core_temp_avg_slope) + 30.0f;
    // }
}

float common_hal_mcu_processor_get_voltage(void) {
//  float adc_read_core_vref(ADC_HandleTypeDef *adcHandle) {
//     uint32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_VREFINT);

//     // update the reference correction factor
//     adc_refcor = ((float)(*VREFIN_CAL)) / ((float)raw_value);

//     return (*VREFIN_CAL) * ADC_SCALE;
// }
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return SystemCoreClock;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    for (int i=0; i<3; i++) {
        ((uint32_t*) raw_id)[i] = STM32_UUID[i];
    }
}
