/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
 * Copyright (c) 2021 Jonathan Hogg
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

#include "py/mphal.h"
#include "adc.h"
#include "driver/adc.h"
#include "esp_adc/adc_cali_scheme.h"

#define DEFAULT_VREF 1100

void madcblock_bits_helper(machine_adc_block_obj_t *self, mp_int_t bits) {
    if (bits < SOC_ADC_RTC_MIN_BITWIDTH && bits > SOC_ADC_RTC_MAX_BITWIDTH) {
        // Invalid value for the current chip, raise exception in the switch below.
        bits = -1;
    }
    switch (bits) {
        case 9:
            self->width = ADC_BITWIDTH_9;
            break;
        case 10:
            self->width = ADC_BITWIDTH_10;
            break;
        case 11:
            self->width = ADC_BITWIDTH_11;
            break;
        case 12:
            self->width = ADC_BITWIDTH_12;
            break;
        case 13:
            self->width = ADC_BITWIDTH_13;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }
    self->bits = bits;

    if (self->unit_id == ADC_UNIT_1) {
        adc1_config_width(self->width);
    }
}

mp_int_t madcblock_read_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id) {
    int raw = 0;
    if (self->unit_id == ADC_UNIT_1) {
        raw = adc1_get_raw(channel_id);
    } else {
        #if (SOC_ADC_PERIPH_NUM >= 2)
        check_esp_err(adc2_get_raw(channel_id, self->width, &raw));
        #endif
    }
    return raw;
}

static esp_err_t ensure_adc_calibration(machine_adc_block_obj_t *self, adc_atten_t atten) {
    if (self->handle[atten] != NULL) {
        return ESP_OK;
    }

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = self->unit_id,
        .atten = atten,
        .bitwidth = self->width,
    };
    return adc_cali_create_scheme_curve_fitting(&cali_config, &self->handle[atten]);
    #else
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = self->unit_id,
        .atten = atten,
        .bitwidth = self->width,
    };
    return adc_cali_create_scheme_line_fitting(&cali_config, &self->handle[atten]);
    #endif
}

mp_int_t madcblock_read_uv_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id, adc_atten_t atten) {
    int raw = madcblock_read_helper(self, channel_id);
    int uv;

    check_esp_err(ensure_adc_calibration(self, atten));
    check_esp_err(adc_cali_raw_to_voltage(self->handle[atten], raw, &uv));

    return (mp_int_t)uv * 1000;
}
