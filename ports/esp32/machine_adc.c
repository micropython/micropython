/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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


#include <stdio.h>

#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

typedef struct _madc_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    adc1_channel_t adc1_id;
} madc_obj_t;

STATIC const madc_obj_t madc_obj[] = {
    {{&machine_adc_type}, GPIO_NUM_36, ADC1_CHANNEL_0},
    {{&machine_adc_type}, GPIO_NUM_37, ADC1_CHANNEL_1},
    {{&machine_adc_type}, GPIO_NUM_38, ADC1_CHANNEL_2},
    {{&machine_adc_type}, GPIO_NUM_39, ADC1_CHANNEL_3},
    {{&machine_adc_type}, GPIO_NUM_32, ADC1_CHANNEL_4},
    {{&machine_adc_type}, GPIO_NUM_33, ADC1_CHANNEL_5},
    {{&machine_adc_type}, GPIO_NUM_34, ADC1_CHANNEL_6},
    {{&machine_adc_type}, GPIO_NUM_35, ADC1_CHANNEL_7},
};

STATIC uint8_t adc_bit_width;

STATIC mp_obj_t madc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {

    static int initialized = 0;
    if (!initialized) {
        adc1_config_width(ADC_WIDTH_12Bit);
        adc_bit_width = 12;
        initialized = 1;
    }

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);
    const madc_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        if (pin_id == madc_obj[i].gpio_id) {
            self = &madc_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Pin for ADC"));
    }
    esp_err_t err = adc1_config_channel_atten(self->adc1_id, ADC_ATTEN_0db);
    if (err == ESP_OK) {
        return MP_OBJ_FROM_PTR(self);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
}

STATIC void madc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    madc_obj_t *self = self_in;
    mp_printf(print, "ADC(Pin(%u))", self->gpio_id);
}

// read_u16()
STATIC mp_obj_t madc_read_u16(mp_obj_t self_in) {
    madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t raw = adc1_get_raw(self->adc1_id);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    uint32_t u16 = raw << (16 - adc_bit_width) | raw >> (2 * adc_bit_width - 16);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_u16_obj, madc_read_u16);

// Legacy method
STATIC mp_obj_t madc_read(mp_obj_t self_in) {
    madc_obj_t *self = self_in;
    int val = adc1_get_raw(self->adc1_id);
    if (val == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}
MP_DEFINE_CONST_FUN_OBJ_1(madc_read_obj, madc_read);

STATIC mp_obj_t madc_atten(mp_obj_t self_in, mp_obj_t atten_in) {
    madc_obj_t *self = self_in;
    adc_atten_t atten = mp_obj_get_int(atten_in);
    esp_err_t err = adc1_config_channel_atten(self->adc1_id, atten);
    if (err == ESP_OK) {
        return mp_const_none;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_atten_obj, madc_atten);

STATIC mp_obj_t madc_width(mp_obj_t cls_in, mp_obj_t width_in) {
    adc_bits_width_t width = mp_obj_get_int(width_in);
    esp_err_t err = adc1_config_width(width);
    if (err != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
    }
    switch (width) {
        case ADC_WIDTH_9Bit:
            adc_bit_width = 9;
            break;
        case ADC_WIDTH_10Bit:
            adc_bit_width = 10;
            break;
        case ADC_WIDTH_11Bit:
            adc_bit_width = 11;
            break;
        case ADC_WIDTH_12Bit:
            adc_bit_width = 12;
            break;
        default:
            break;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_width_fun_obj, madc_width);
MP_DEFINE_CONST_CLASSMETHOD_OBJ(madc_width_obj, MP_ROM_PTR(&madc_width_fun_obj));

STATIC const mp_rom_map_elem_t madc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&madc_read_u16_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&madc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_atten), MP_ROM_PTR(&madc_atten_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&madc_width_obj) },

    { MP_ROM_QSTR(MP_QSTR_ATTN_0DB), MP_ROM_INT(ADC_ATTEN_0db) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_2_5DB), MP_ROM_INT(ADC_ATTEN_2_5db) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_6DB), MP_ROM_INT(ADC_ATTEN_6db) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_11DB), MP_ROM_INT(ADC_ATTEN_11db) },

    { MP_ROM_QSTR(MP_QSTR_WIDTH_9BIT), MP_ROM_INT(ADC_WIDTH_9Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_10BIT), MP_ROM_INT(ADC_WIDTH_10Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_11BIT), MP_ROM_INT(ADC_WIDTH_11Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_12BIT), MP_ROM_INT(ADC_WIDTH_12Bit) },
};

STATIC MP_DEFINE_CONST_DICT(madc_locals_dict, madc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = madc_print,
    .make_new = madc_make_new,
    .locals_dict = (mp_obj_t)&madc_locals_dict,
};
