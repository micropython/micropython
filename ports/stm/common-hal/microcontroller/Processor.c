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
#include "py/runtime.h"

#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif
#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "supervisor/shared/translate.h"

#include STM32_HAL_H

#if CPY_STM32F4

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

// Factory calibration locations
#define ADC_CAL_ADDRESS         (0x1fff7a2a)
#define ADC_CAL1                ((uint16_t *)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t *)(ADC_CAL_ADDRESS + 4))
#define VREFIN_CAL ((uint16_t *)ADC_CAL_ADDRESS)

// correction factor for reference value
STATIC volatile float adc_refcor = 1.0f;

STATIC void set_adc_params(ADC_HandleTypeDef *AdcHandle) {
    AdcHandle->Instance = ADC1;
    AdcHandle->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    AdcHandle->Init.Resolution = ADC_RESOLUTION_12B;
    AdcHandle->Init.ScanConvMode = DISABLE;
    AdcHandle->Init.ContinuousConvMode = DISABLE;
    AdcHandle->Init.DiscontinuousConvMode = DISABLE;
    AdcHandle->Init.NbrOfDiscConversion = 0;
    AdcHandle->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    AdcHandle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle->Init.NbrOfConversion = 1;
    AdcHandle->Init.DMAContinuousRequests = DISABLE;
    AdcHandle->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
}

#endif

float common_hal_mcu_processor_get_temperature(void) {
    #if CPY_STM32F4
    __HAL_RCC_ADC1_CLK_ENABLE();

    // HAL Implementation
    ADC_HandleTypeDef AdcHandle;
    ADC_ChannelConfTypeDef sConfig;
    set_adc_params(&AdcHandle);
    HAL_ADC_Init(&AdcHandle);

    ADC->CCR |= ADC_CCR_TSVREFE;
    ADC->CCR &= ~ADC_CCR_VBATE; // If this somehow got turned on, it'll return bad values.

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR; // either 16 or 18, depending on chip
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // Temp sensor likes 10us minimum
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADC_Start(&AdcHandle);
    if (HAL_ADC_PollForConversion(&AdcHandle,1) != HAL_OK) {
        mp_raise_RuntimeError(translate("Temperature read timed out"));
    }
    uint32_t value = (uint32_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    // There's no F4 specific appnote for this but it works the same as the L1 in AN3964
    float core_temp_avg_slope = (*ADC_CAL2 - *ADC_CAL1) / 80.0;
    return (((float)value * adc_refcor - *ADC_CAL1) / core_temp_avg_slope) + 30.0f;
    #else
    return false;
    #endif
}

float common_hal_mcu_processor_get_voltage(void) {
    #if CPY_STM32F4
    __HAL_RCC_ADC1_CLK_ENABLE();

    // HAL Implementation
    ADC_HandleTypeDef AdcHandle;
    ADC_ChannelConfTypeDef sConfig;
    set_adc_params(&AdcHandle);
    HAL_ADC_Init(&AdcHandle);

    ADC->CCR |= ADC_CCR_TSVREFE;

    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADC_Start(&AdcHandle);
    if (HAL_ADC_PollForConversion(&AdcHandle,1) != HAL_OK) {
        mp_raise_RuntimeError(translate("Voltage read timed out"));
    }
    uint32_t value = (uint32_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    // This value could be used to actively correct ADC values.
    adc_refcor = ((float)(*VREFIN_CAL)) / ((float)value);

    return adc_refcor * 3.3f;
    #else
    return false;
    #endif
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return SystemCoreClock;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    #if CPY_STM32F4
    for (int i = 0; i < 3; i++) {
        ((uint32_t *)raw_id)[i] = STM32_UUID[i];
    }
    #endif
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    #if CIRCUITPY_ALARM
    if (alarm_get_wakeup_cause() != STM_WAKEUP_UNDEF) {
        return RESET_REASON_DEEP_SLEEP_ALARM;
    }
    #endif
    return RESET_REASON_UNKNOWN;
}
