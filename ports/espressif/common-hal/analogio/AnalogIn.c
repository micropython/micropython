// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/analogio/AnalogIn.h"

#include "bindings/espidf/__init__.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "adc_cali_schemes.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "driver/gpio.h"
#include "hal/adc_types.h"

#include "shared-bindings/microcontroller/Pin.h"

#include <string.h>

#define DEFAULT_VREF        1100
#define NO_OF_SAMPLES       2
#define ATTENUATION         ADC_ATTEN_DB_11
#if defined(CONFIG_IDF_TARGET_ESP32)
#define DATA_WIDTH          ADC_BITWIDTH_12
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
#define DATA_WIDTH          ADC_BITWIDTH_12
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define DATA_WIDTH          ADC_BITWIDTH_12
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#define DATA_WIDTH          ADC_BITWIDTH_12
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define DATA_WIDTH          ADC_BITWIDTH_13
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define DATA_WIDTH          ADC_BITWIDTH_12
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
#define DATA_WIDTH          ADC_BITWIDTH_12
#else
#error No known CONFIG_IDF_TARGET_xxx found
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (pin->adc_index == NO_ADC || pin->adc_channel == NO_ADC_CHANNEL) {
        raise_ValueError_invalid_pin();
    }
    common_hal_mcu_pin_claim(pin);
    self->pin = pin;
    // Pull-ups are enabled by default for power-saving reasons on quiescent pins.
    // Turn off the pull-up as soon as we know the pin will be used for analog reads,
    // since it may take a while for the voltage to stabilize if the input is high-impedance.
    gpio_pullup_dis(pin->number);
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = self->pin->adc_index,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    CHECK_ESP_RESULT(adc_oneshot_new_unit(&adc_config, &adc_handle));

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ATTENUATION,
        .bitwidth = DATA_WIDTH
    };
    adc_channel_t channel = (adc_channel_t)self->pin->adc_channel;
    adc_oneshot_config_channel(adc_handle, channel, &channel_config);

    adc_cali_scheme_ver_t supported_schemes;
    adc_cali_check_scheme(&supported_schemes);
    adc_cali_scheme_ver_t calibration_scheme = 0;
    adc_cali_handle_t calibration;

    #if defined(ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t config = {
        .unit_id = self->pin->adc_index,
        .chan = channel,
        .atten = ATTENUATION,
        .bitwidth = DATA_WIDTH
    };
    if (adc_cali_create_scheme_curve_fitting(&config, &calibration) == ESP_OK) {
        calibration_scheme = ADC_CALI_SCHEME_VER_CURVE_FITTING;
    }
    #endif
    #if defined(ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (calibration_scheme == 0) {
        adc_cali_line_fitting_config_t config = {
            .unit_id = self->pin->adc_index,
            .atten = ATTENUATION,
            .bitwidth = DATA_WIDTH,
            #ifdef CONFIG_IDF_TARGET_ESP32
            .default_vref = DEFAULT_VREF,
            #endif
        };
        if (adc_cali_create_scheme_line_fitting(&config, &calibration) == ESP_OK) {
            calibration_scheme = ADC_CALI_SCHEME_VER_LINE_FITTING;
        }
    }
    #endif

    uint32_t adc_reading = 0;
    size_t sample_count = 0;
    // Multisampling
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        int raw;
        ret = adc_oneshot_read(adc_handle, channel, &raw);
        if (ret != ESP_OK) {
            continue;
        }
        adc_reading += raw;
        sample_count += 1;
    }
    if (sample_count == 0) {
        raise_esp_error(ret);
    }
    adc_reading /= sample_count;

    // This corrects non-linear regions of the ADC range with a LUT, so it's a better reading than raw
    int voltage;
    adc_cali_raw_to_voltage(calibration, adc_reading, &voltage);


    #if defined(ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (calibration_scheme == ADC_CALI_SCHEME_VER_CURVE_FITTING) {
        adc_cali_delete_scheme_curve_fitting(calibration);
    }
    #endif
    #if defined(ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (calibration_scheme == ADC_CALI_SCHEME_VER_LINE_FITTING) {
        adc_cali_delete_scheme_line_fitting(calibration);
    }
    #endif
    adc_oneshot_del_unit(adc_handle);
    return voltage * ((1 << 16) - 1) / 3300;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
