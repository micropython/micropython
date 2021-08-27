/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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
#include "shared-bindings/microcontroller/Pin.h"

#include <string.h>

#include "py/runtime.h"

#include "fsl_adc.h"

#define ADC_CHANNEL_GROUP 0

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    adc_config_t config = {0};

    if (pin->adc == NULL) {
        mp_raise_ValueError(translate("Pin does not have ADC capabilities"));
    }

    ADC_GetDefaultConfig(&config);

    config.enableLongSample = true;
    config.samplePeriodMode = kADC_SamplePeriod8or24Clocks;

    ADC_Init(pin->adc, &config);
    ADC_SetHardwareAverageConfig(pin->adc, kADC_HardwareAverageCount32);
    ADC_DoAutoCalibration(pin->adc);

    claim_pin(pin);

    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    adc_channel_config_t config = { 0 };
    config.channelNumber = self->pin->adc_channel;

    ADC_SetChannelConfig(self->pin->adc, ADC_CHANNEL_GROUP, &config);

    while (!ADC_GetChannelStatusFlags(self->pin->adc, ADC_CHANNEL_GROUP)) {

    }

    // Shift the value to be 16 bit
    return ADC_GetChannelConversionValue(self->pin->adc, ADC_CHANNEL_GROUP) << 4;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
