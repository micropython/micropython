// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

#include "nrf_saadc.h"
#include "nrf_gpio.h"

#define CHANNEL_NO 0

void analogin_init(void) {
    // Calibrate the ADC once, on startup.
    nrf_saadc_enable(NRF_SAADC);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE);
    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_CALIBRATEOFFSET);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE) == 0) {
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE);
    nrf_saadc_disable(NRF_SAADC);
}

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->adc_channel == 0) {
        raise_ValueError_invalid_pin();
    }

    nrf_gpio_cfg_default(pin->number);

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

    nrf_gpio_cfg_default(self->pin->number);

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.

    nrf_saadc_value_t value = 0;

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain = NRF_SAADC_GAIN1_4,
        .reference = NRF_SAADC_REFERENCE_VDD4,
        .acq_time = NRF_SAADC_ACQTIME_3US,
        .mode = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst = NRF_SAADC_BURST_DISABLED
    };

    nrf_saadc_resolution_set(NRF_SAADC, NRF_SAADC_RESOLUTION_14BIT);
    nrf_saadc_oversample_set(NRF_SAADC, NRF_SAADC_OVERSAMPLE_DISABLED);
    nrf_saadc_enable(NRF_SAADC);

    for (uint32_t i = 0; i < SAADC_CH_NUM; i++) {
        nrf_saadc_channel_input_set(NRF_SAADC, i, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);
    }

    nrf_saadc_channel_init(NRF_SAADC, CHANNEL_NO, &config);
    nrf_saadc_channel_input_set(NRF_SAADC, CHANNEL_NO, self->pin->adc_channel, self->pin->adc_channel);
    nrf_saadc_buffer_init(NRF_SAADC, &value, 1);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_START);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STARTED) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STARTED);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_END) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_END);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_STOP);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STOPPED) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STOPPED);

    nrf_saadc_disable(NRF_SAADC);

    // Adding the "asm volatile" memory fence here or anywhere after the declaration of `value`
    // fixes an issue with gcc13 which causes `value` to always be zero.
    // Compiling with gcc10 or gcc12 is fine.
    // It can also be fixed by declaring `value` to be static.
    // I think I'd like to declare `value` as volatile, but that causes type errors.
    asm volatile ("" : : : "memory");

    // Disconnect ADC from pin.
    nrf_saadc_channel_input_set(NRF_SAADC, CHANNEL_NO, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);

    // value is signed and might be (slightly) < 0, even on single-ended conversions, so force to 0.
    if (value < 0) {
        value = 0;
    }

    // Stretch 14-bit ADC reading to 16-bit range
    return (value << 2) | (value >> 12);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    // The nominal VCC voltage
    return 3.3f;
}
