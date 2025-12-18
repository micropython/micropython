/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc_block.c via MICROPY_PY_MACHINE_ADC_BLOCK_INCLUDEFILE.

#include "py/mphal.h"
#include "adc.h"
#include "esp_adc/adc_oneshot.h"

machine_adc_block_obj_t madcblock_obj[] = {
    {{&machine_adc_block_type}, ADC_UNIT_1, NULL, ADC_WIDTH_MAX, {0}},
    #if SOC_ADC_PERIPH_NUM > 1
    {{&machine_adc_block_type}, ADC_UNIT_2, NULL, ADC_WIDTH_MAX, {0}},
    #endif
};

static void mp_machine_adc_block_print(const mp_print_t *print, machine_adc_block_obj_t *self) {
    mp_printf(print, "ADCBlock(%u, bits=%u)", self->unit_id, self->bitwidth);
}

static void mp_machine_adc_block_bits_set(machine_adc_block_obj_t *self, mp_int_t bits) {
    mp_machine_adc_block_width_set_helper(self, bits);
}

static machine_adc_block_obj_t *mp_machine_adc_block_get(mp_int_t unit) {
    for (int i = 0; i < MP_ARRAY_SIZE(madcblock_obj); i++) {
        if (unit == madcblock_obj[i].unit_id) {
            return &madcblock_obj[i];
        }
    }
    return NULL;
}

static machine_adc_obj_t *mp_machine_adc_block_connect(machine_adc_block_obj_t *self, mp_int_t channel_id, mp_hal_pin_obj_t gpio_id, mp_map_t *kw_args) {
    const machine_adc_obj_t *adc = madc_search_helper(self, channel_id, gpio_id);
    if (adc == NULL) {
        return NULL;
    }
    madc_init_helper(adc, 0, NULL, kw_args);
    return (machine_adc_obj_t *)adc;
}
