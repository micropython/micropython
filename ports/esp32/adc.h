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
#include "esp_adc_cal.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN_MAX SOC_ADC_ATTEN_NUM

typedef struct _machine_adc_block_obj_t {
    mp_obj_base_t base;
    adc_unit_t unit_id;
    mp_int_t bits;
    adc_bits_width_t width;
    adc_cali_handle_t handle[ADC_ATTEN_MAX];
} machine_adc_block_obj_t;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    machine_adc_block_obj_t *block;
    adc_channel_t channel_id;
    gpio_num_t gpio_id;
} machine_adc_obj_t;

extern machine_adc_block_obj_t madcblock_obj[];

void madcblock_bits_helper(machine_adc_block_obj_t *self, mp_int_t bits);
mp_int_t madcblock_read_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id);
mp_int_t madcblock_read_uv_helper(machine_adc_block_obj_t *self, adc_channel_t channel_id, adc_atten_t atten);

const machine_adc_obj_t *madc_search_helper(machine_adc_block_obj_t *block, adc_channel_t channel_id, gpio_num_t gpio_id);
void madc_init_helper(const machine_adc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

#endif // MICROPY_INCLUDED_ESP32_ADC_H
