/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/analogio/AnalogIn.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "components/driver/include/driver/adc_common.h"
#include "components/esp_adc_cal/include/esp_adc_cal.h"

#include "shared-bindings/microcontroller/Pin.h"

#include <string.h>

#define DEFAULT_VREF        1100
#define NO_OF_SAMPLES       2
#define ATTENUATION         ADC_ATTEN_DB_11
#ifdef CONFIG_IDF_TARGET_ESP32C3
#define DATA_WIDTH          ADC_WIDTH_BIT_12
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define DATA_WIDTH          ADC_WIDTH_BIT_13
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define DATA_WIDTH          ADC_WIDTH_BIT_12
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (pin->adc_index == 0 || pin->adc_channel == ADC_CHANNEL_MAX) {
        mp_raise_ValueError(translate("Pin does not have ADC capabilities"));
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
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    if (self->pin->adc_index == ADC_UNIT_1) {
        adc1_config_width(DATA_WIDTH);
        adc1_config_channel_atten((adc1_channel_t)self->pin->adc_channel, ATTENUATION);
    } else if (self->pin->adc_index == ADC_UNIT_2) {
        adc2_config_channel_atten((adc2_channel_t)self->pin->adc_channel, ATTENUATION);
    } else {
        mp_raise_ValueError(translate("Invalid Pin"));
    }

    // Automatically select calibration process depending on status of efuse
    esp_adc_cal_characteristics_t adc_chars;
    memset(&adc_chars, 0, sizeof(adc_chars));
    esp_adc_cal_characterize(self->pin->adc_index, ATTENUATION, DATA_WIDTH, DEFAULT_VREF, &adc_chars);

    uint32_t adc_reading = 0;
    // Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (self->pin->adc_index == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)self->pin->adc_channel);
        } else {
            int raw;
            esp_err_t r = adc2_get_raw((adc2_channel_t)self->pin->adc_channel, DATA_WIDTH, &raw);
            if (r != ESP_OK) {
                mp_raise_ValueError(translate("ADC2 is being used by WiFi"));
            }
            adc_reading += raw;
        }
    }
    adc_reading /= NO_OF_SAMPLES;

    // This corrects non-linear regions of the ADC range with a LUT, so it's a better reading than raw
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);
    return voltage * ((1 << 16) - 1) / 3300;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
