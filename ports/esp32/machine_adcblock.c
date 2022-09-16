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

#include <stdio.h>

#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "machine_adc.h"
#include "machine_adcblock.h"

#define DEFAULT_VREF 1100

madcblock_obj_t madcblock_obj[] = {
    #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    {{&machine_adcblock_type}, ADC_UNIT_1, 12, -1, {0}},
    {{&machine_adcblock_type}, ADC_UNIT_2, 12, -1, {0}},
    #elif CONFIG_IDF_TARGET_ESP32S2
    {{&machine_adcblock_type}, ADC_UNIT_1, 13, -1, {0}},
    {{&machine_adcblock_type}, ADC_UNIT_2, 13, -1, {0}},
    #endif
};

STATIC void madcblock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    madcblock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADCBlock(%u, bits=%u)", self->unit_id, self->bits);
}

void madcblock_bits_helper(madcblock_obj_t *self, mp_int_t bits) {
    switch (bits) {
        #if CONFIG_IDF_TARGET_ESP32
        case 9:
            self->width = ADC_WIDTH_BIT_9;
            break;
        case 10:
            self->width = ADC_WIDTH_BIT_10;
            break;
        case 11:
            self->width = ADC_WIDTH_BIT_11;
            break;
        #endif
        #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
        case 12:
            self->width = ADC_WIDTH_BIT_12;
            break;
        #endif
        #if CONFIG_IDF_TARGET_ESP32S2
        case 13:
            self->width = ADC_WIDTH_BIT_13;
            break;
        #endif
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }
    self->bits = bits;

    if (self->unit_id == ADC_UNIT_1) {
        adc1_config_width(self->width);
    }
    for (adc_atten_t atten = ADC_ATTEN_DB_0; atten < ADC_ATTEN_MAX; atten++) {
        if (self->characteristics[atten] != NULL) {
            esp_adc_cal_characterize(self->unit_id, atten, self->width, DEFAULT_VREF, self->characteristics[atten]);
        }
    }
}

STATIC void madcblock_init_helper(madcblock_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_bits,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t bits = args[ARG_bits].u_int;
    if (bits != -1) {
        madcblock_bits_helper(self, bits);
    } else if (self->width == -1) {
        madcblock_bits_helper(self, self->bits);
    }
}

STATIC mp_obj_t madcblock_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    adc_unit_t unit = mp_obj_get_int(args[0]);
    madcblock_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(madcblock_obj); i++) {
        if (unit == madcblock_obj[i].unit_id) {
            self = &madcblock_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid block id"));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    madcblock_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t madcblock_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    madcblock_obj_t *self = pos_args[0];
    madcblock_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(madcblock_init_obj, 1, madcblock_init);

STATIC mp_obj_t madcblock_connect(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    madcblock_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    adc_channel_t channel_id = -1;
    gpio_num_t gpio_id = -1;
    if (n_pos_args == 2) {
        if (mp_obj_is_int(pos_args[1])) {
            channel_id = mp_obj_get_int(pos_args[1]);
        } else {
            gpio_id = machine_pin_get_id(pos_args[1]);
        }
    } else if (n_pos_args == 3) {
        channel_id = mp_obj_get_int(pos_args[1]);
        gpio_id = machine_pin_get_id(pos_args[2]);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("too many positional args"));
    }

    const madc_obj_t *adc = madc_search_helper(self, channel_id, gpio_id);
    if (adc != NULL) {
        madc_init_helper(adc, 0, pos_args + n_pos_args, kw_args);
        return MP_OBJ_FROM_PTR(adc);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("no matching ADC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(madcblock_connect_obj, 2, madcblock_connect);

mp_int_t madcblock_read_helper(madcblock_obj_t *self, adc_channel_t channel_id) {
    int raw;
    if (self->unit_id == ADC_UNIT_1) {
        raw = adc1_get_raw(channel_id);
    } else {
        check_esp_err(adc2_get_raw(channel_id, self->width, &raw));
    }
    return raw;
}

mp_int_t madcblock_read_uv_helper(madcblock_obj_t *self, adc_channel_t channel_id, adc_atten_t atten) {
    int raw = madcblock_read_helper(self, channel_id);
    esp_adc_cal_characteristics_t *adc_chars = self->characteristics[atten];
    if (adc_chars == NULL) {
        adc_chars = malloc(sizeof(esp_adc_cal_characteristics_t));
        esp_adc_cal_characterize(self->unit_id, atten, self->width, DEFAULT_VREF, adc_chars);
        self->characteristics[atten] = adc_chars;
    }
    mp_int_t uv = esp_adc_cal_raw_to_voltage(raw, adc_chars) * 1000;
    return uv;
}

STATIC const mp_rom_map_elem_t madcblock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&madcblock_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&madcblock_connect_obj) },
};
STATIC MP_DEFINE_CONST_DICT(madcblock_locals_dict, madcblock_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adcblock_type,
    MP_QSTR_ADCBlock,
    MP_TYPE_FLAG_NONE,
    make_new, madcblock_make_new,
    print, madcblock_print,
    locals_dict, &madcblock_locals_dict
    );
