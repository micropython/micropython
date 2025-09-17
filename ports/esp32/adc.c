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
#include "py/mperrno.h"
#include "adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"

static esp_err_t ensure_adc_calibration(machine_adc_block_obj_t *self, adc_atten_t atten);

void adc_is_init_guard(machine_adc_block_obj_t *self) {
    if (!self->handle) {
        mp_raise_OSError(MP_EPERM);
    }
}

esp_err_t apply_self_adc_channel_atten(const machine_adc_obj_t *self, uint8_t atten) {
    adc_is_init_guard(self->block);

    adc_oneshot_chan_cfg_t config = {
        .atten = atten,
        .bitwidth = self->block->bitwidth,
    };
    esp_err_t ret = adc_oneshot_config_channel(self->block->handle, self->channel_id, &config);
    return ret;
}

mp_int_t madcblock_read_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id) {
    adc_is_init_guard(self);

    int reading = 0;
    adc_oneshot_read(self->handle, channel_id, &reading);
    return reading;
}

/*
During testing, it turned out that the function `adc_cali_raw_to_voltage` does not account for the lower resolution,
instead it expects the full resolution value as an argument, hence the scaling applied here
*/
mp_int_t madcblock_read_uv_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id, adc_atten_t atten) {
    int raw = madcblock_read_helper(self, channel_id);
    int uv = 0;

    check_esp_err(ensure_adc_calibration(self, atten));
    check_esp_err(adc_cali_raw_to_voltage(self->calib[atten], (raw << (ADC_WIDTH_MAX - self->bitwidth)), &uv));
    return (mp_int_t)uv * 1000;
}

static esp_err_t ensure_adc_calibration(machine_adc_block_obj_t *self, adc_atten_t atten) {
    if (self->calib[atten] != NULL) {
        return ESP_OK;
    }

    esp_err_t ret = ESP_OK;

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = self->unit_id,
        .atten = atten,
        .bitwidth = self->bitwidth,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &self->calib[atten]);
    #else
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = self->unit_id,
        .atten = atten,
        .bitwidth = self->bitwidth,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &self->calib[atten]);
    #endif

    return ret;
}
