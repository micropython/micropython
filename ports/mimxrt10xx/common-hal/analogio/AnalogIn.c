// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "common-hal/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include <string.h>

#include "py/runtime.h"

#include "sdk/drivers/adc_12b1msps_sar/fsl_adc.h"

#define ADC_CHANNEL_GROUP 0

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    adc_config_t config = {0};

    if (pin->adc == NULL) {
        raise_ValueError_invalid_pin();
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

    uint16_t value = ADC_GetChannelConversionValue(self->pin->adc, ADC_CHANNEL_GROUP);

    // Stretch 12-bit ADC reading to 16-bit range
    return (value << 4) | (value >> 8);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
