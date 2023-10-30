/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/analogio/AnalogIn.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#if CPY_STM32L4
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_bus.h"
#define ADC_SAMPLETIME ADC_SAMPLETIME_24CYCLES_5
#define LL_APB2_GRP1_PERIPH_ADC1 LL_AHB2_GRP1_PERIPH_ADC
#else
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
#define ADC_SAMPLETIME ADC_SAMPLETIME_15CYCLES
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {

    // No ADC function on pin
    if (pin->adc_unit == 0x00) {
        raise_ValueError_invalid_pin();
    }
    // TODO: add ADC traits to structure?

    // Note that ADC2 is always bundled pin-to-pin with ADC1 if it exists, and used only
    // for dual conversion. For this basic application it is never used.
    LL_GPIO_SetPinMode(pin_port(pin->port), (uint32_t)pin_mask(pin->number), LL_GPIO_MODE_ANALOG);
    if (pin->adc_unit & 0x01) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    } else if (pin->adc_unit == 0x04) {
        #ifdef LL_APB2_GRP1_PERIPH_ADC3
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3);
        #endif
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid ADC Unit value"));
    }
    common_hal_mcu_pin_claim(pin);
    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->port, self->pin->number);
    self->pin = NULL;
}

uint32_t adc_channel(uint32_t channel) {
    #if CPY_STM32L4
    switch (channel) {
        case 0:
            return ADC_CHANNEL_0;
        case 1:
            return ADC_CHANNEL_1;
        case 2:
            return ADC_CHANNEL_2;
        case 3:
            return ADC_CHANNEL_3;
        case 4:
            return ADC_CHANNEL_4;
        case 5:
            return ADC_CHANNEL_5;
        case 6:
            return ADC_CHANNEL_6;
        case 7:
            return ADC_CHANNEL_7;
        case 8:
            return ADC_CHANNEL_8;
        case 9:
            return ADC_CHANNEL_9;
        case 10:
            return ADC_CHANNEL_10;
        case 11:
            return ADC_CHANNEL_11;
        case 12:
            return ADC_CHANNEL_12;
        case 13:
            return ADC_CHANNEL_13;
        case 14:
            return ADC_CHANNEL_14;
        case 15:
            return ADC_CHANNEL_15;
        case 16:
            return ADC_CHANNEL_16;
        case 17:
            return ADC_CHANNEL_17;
        case 18:
            return ADC_CHANNEL_18;
        default:
            return 0;
    }
    #else
    return channel;
    #endif
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.
    ADC_TypeDef *ADCx;

    if (self->pin->adc_unit & 0x01) {
        ADCx = ADC1;
        #if CPY_STM32L4
        __HAL_RCC_ADC_CLK_ENABLE();
        #endif
    } else if (self->pin->adc_unit == 0x04) {
        #ifdef ADC3
        ADCx = ADC3;
        #endif
    } else {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid ADC Unit value"));
    }

    LL_GPIO_SetPinMode(pin_port(self->pin->port), (uint32_t)pin_mask(self->pin->number), LL_GPIO_MODE_ANALOG);
    // LL_GPIO_PIN_0

    // HAL Implementation
    ADC_HandleTypeDef AdcHandle = {};
    ADC_ChannelConfTypeDef sConfig = {};

    AdcHandle.Instance = ADCx;
    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
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
    #ifdef ADC_OVR_DATA_OVERWRITTEN
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                    /* DR register is overwritten with the last conversion result in case of overrun */
    #endif

    if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
        return 0;
    }

    sConfig.Channel = adc_channel(self->pin->adc_channel); // ADC_CHANNEL_0 <-normal iteration, not mask
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME;
    #if CPY_STM32L4
    sConfig.SingleDiff = ADC_SINGLE_ENDED;                   /* Single-ended input channel */
    sConfig.OffsetNumber = ADC_OFFSET_NONE;                  /* No offset subtraction */
    if (!IS_ADC_CHANNEL(&AdcHandle, sConfig.Channel)) {
        return 0;
    }
    #endif
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
        return 0;
    }
    #if CPY_STM32L4
    if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK) {
        return 0;
    }
    #endif
    if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
        return 0;
    }
    HAL_ADC_PollForConversion(&AdcHandle, 1);
    uint16_t value = (uint16_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    // Stretch 12-bit ADC reading to 16-bit range
    return (value << 4) | (value >> 8);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
