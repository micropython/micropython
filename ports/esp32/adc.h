/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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
#ifndef MICROPY_INCLUDED_ESP32_ADC_H
#define MICROPY_INCLUDED_ESP32_ADC_H

#include "py/runtime.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN_COUNT SOC_ADC_ATTEN_NUM
#define ADC_ATTEN_MIN ADC_ATTEN_DB_0
#define ADC_ATTEN_MAX ADC_ATTEN_DB_11

/*
https://github.com/espressif/esp-idf/issues/13128
https://github.com/espressif/esp-idf/blob/release/v5.2/components/soc/esp32s3/include/soc/soc_caps.h
https://docs.espressif.com/projects/esp-chip-errata/en/latest/esp32s2/03-errata-description/index.html
https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

Looks like only the original esp32 is capable of bitwidth adjustment, all others are stuck at 12 bits,
except the S2, which is locked at 13 bits, otherwise attenuation doesn't work.
*/
#if CONFIG_IDF_TARGET_ESP32S2

#define ADC_WIDTH_MIN ADC_BITWIDTH_13
#define ADC_WIDTH_MAX ADC_BITWIDTH_13

#elif CONFIG_IDF_TARGET_ESP32

#define ADC_WIDTH_MIN ADC_BITWIDTH_9
#define ADC_WIDTH_MAX ADC_BITWIDTH_12

#else

#define ADC_WIDTH_MIN ADC_BITWIDTH_12
#define ADC_WIDTH_MAX ADC_BITWIDTH_12

#endif

typedef struct _machine_adc_block_obj_t {
    mp_obj_base_t base;
    adc_unit_t unit_id;
    adc_oneshot_unit_handle_t handle;
    adc_bitwidth_t bitwidth;
    adc_cali_handle_t calib[ADC_ATTEN_COUNT];
} machine_adc_block_obj_t;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    machine_adc_block_obj_t *block;
    adc_channel_t channel_id;
    gpio_num_t gpio_id;
} machine_adc_obj_t;

extern machine_adc_block_obj_t madcblock_obj[];

esp_err_t apply_self_adc_channel_atten(const machine_adc_obj_t *self, uint8_t atten);

mp_int_t madcblock_read_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id);
mp_int_t madcblock_read_uv_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id, adc_atten_t atten);

const machine_adc_obj_t *madc_search_helper(machine_adc_block_obj_t *block, adc_channel_t channel_id, gpio_num_t gpio_id);
void madc_init_helper(const machine_adc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

mp_int_t mp_machine_adc_atten_get_helper(const machine_adc_obj_t *self);
void mp_machine_adc_atten_set_helper(const machine_adc_obj_t *self, mp_int_t atten);

mp_int_t mp_machine_adc_width_get_helper(const machine_adc_obj_t *self);
void mp_machine_adc_block_width_set_helper(machine_adc_block_obj_t *self, mp_int_t width);

#endif // MICROPY_INCLUDED_ESP32_ADC_H
