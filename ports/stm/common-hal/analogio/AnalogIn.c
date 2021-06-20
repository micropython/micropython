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
#include "supervisor/shared/translate.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {

    // No ADC function on pin
    if (pin->adc_unit == 0x00) {
        mp_raise_ValueError(translate("Pin does not have ADC capabilities"));
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
        mp_raise_ValueError(translate("Invalid ADC Unit value"));
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
    reset_pin_number(self->pin->port,self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.
    ADC_TypeDef *ADCx;

    if (self->pin->adc_unit & 0x01) {
        ADCx = ADC1;
    } else if (self->pin->adc_unit == 0x04) {
        #ifdef ADC3
        ADCx = ADC3;
        #endif
    } else {
        mp_raise_ValueError(translate("Invalid ADC Unit value"));
    }

    LL_GPIO_SetPinMode(pin_port(self->pin->port), (uint32_t)pin_mask(self->pin->number), LL_GPIO_MODE_ANALOG);
    // LL_GPIO_PIN_0

    // HAL Implementation
    ADC_HandleTypeDef AdcHandle;
    ADC_ChannelConfTypeDef sConfig;

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
    HAL_ADC_Init(&AdcHandle);

    sConfig.Channel = (uint32_t)self->pin->adc_channel; // ADC_CHANNEL_0 <-normal iteration, not mask
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES; // Taken from micropython
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADC_Start(&AdcHandle);
    HAL_ADC_PollForConversion(&AdcHandle,1);
    uint16_t value = (uint16_t)HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    // // Shift the value to be 16 bit.
    return value << 4;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
