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

#include <stdio.h>

#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "machine_adc.h"

#define ADCBLOCK1 (&madcblock_obj[0])
#define ADCBLOCK2 (&madcblock_obj[1])

STATIC const madc_obj_t madc_obj[] = {
    #if CONFIG_IDF_TARGET_ESP32
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_0, GPIO_NUM_36},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_1, GPIO_NUM_37},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_2, GPIO_NUM_38},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_3, GPIO_NUM_39},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_4, GPIO_NUM_32},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_5, GPIO_NUM_33},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_6, GPIO_NUM_34},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_7, GPIO_NUM_35},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_0, GPIO_NUM_4},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_1, GPIO_NUM_0},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_2, GPIO_NUM_2},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_3, GPIO_NUM_15},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_4, GPIO_NUM_13},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_5, GPIO_NUM_12},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_6, GPIO_NUM_14},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_7, GPIO_NUM_27},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_8, GPIO_NUM_25},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_9, GPIO_NUM_26},
    #elif CONFIG_IDF_TARGET_ESP32C3
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_0, GPIO_NUM_0},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_1, GPIO_NUM_1},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_2, GPIO_NUM_2},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_3, GPIO_NUM_3},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_4, GPIO_NUM_4},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_0, GPIO_NUM_5},
    #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_0, GPIO_NUM_1},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_1, GPIO_NUM_2},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_2, GPIO_NUM_3},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_3, GPIO_NUM_4},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_4, GPIO_NUM_5},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_5, GPIO_NUM_6},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_6, GPIO_NUM_7},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_7, GPIO_NUM_8},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_8, GPIO_NUM_9},
    {{&machine_adc_type}, ADCBLOCK1, ADC_CHANNEL_9, GPIO_NUM_10},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_0, GPIO_NUM_11},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_1, GPIO_NUM_12},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_2, GPIO_NUM_13},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_3, GPIO_NUM_14},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_4, GPIO_NUM_15},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_5, GPIO_NUM_16},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_6, GPIO_NUM_17},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_7, GPIO_NUM_18},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_8, GPIO_NUM_19},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_9, GPIO_NUM_20},
    #endif
};

// These values are initialised to 0, which means the corresponding ADC channel is not initialised.
// The madc_atten_get/madc_atten_set functions store (atten+1) here so that the uninitialised state
// can be distinguished from the initialised state.
STATIC uint8_t madc_obj_atten[MP_ARRAY_SIZE(madc_obj)];

static inline adc_atten_t madc_atten_get(const madc_obj_t *self) {
    uint8_t value = madc_obj_atten[self - &madc_obj[0]];
    return value == 0 ? ADC_ATTEN_MAX : value - 1;
}

static inline void madc_atten_set(const madc_obj_t *self, adc_atten_t atten) {
    madc_obj_atten[self - &madc_obj[0]] = atten + 1;
}

const madc_obj_t *madc_search_helper(madcblock_obj_t *block, adc_channel_t channel_id, gpio_num_t gpio_id) {
    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        const madc_obj_t *adc = &madc_obj[i];
        if ((block == NULL || block == adc->block) && (channel_id == -1 || channel_id == adc->channel_id) && (gpio_id == -1 || gpio_id == adc->gpio_id)) {
            return adc;
        }
    }
    return NULL;
}

STATIC void madc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC(Pin(%u), atten=%u)", self->gpio_id, madc_atten_get(self));
}

STATIC void madc_atten_helper(const madc_obj_t *self, mp_int_t atten) {
    esp_err_t err;
    if (self->block->unit_id == ADC_UNIT_1) {
        err = adc1_config_channel_atten(self->channel_id, atten);
    } else {
        err = adc2_config_channel_atten(self->channel_id, atten);
    }
    if (err != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid atten"));
    }
    madc_atten_set(self, atten);
}

void madc_init_helper(const madc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_atten,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_atten, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t atten = args[ARG_atten].u_int;
    if (atten != -1) {
        madc_atten_helper(self, atten);
    } else if (madc_atten_get(self) == ADC_ATTEN_MAX) {
        madc_atten_helper(self, ADC_ATTEN_DB_0);
    }
}

STATIC mp_obj_t madc_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    gpio_num_t gpio_id = machine_pin_get_id(args[0]);
    const madc_obj_t *self = madc_search_helper(NULL, -1, gpio_id);
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    if (self->block->width == -1) {
        madcblock_bits_helper(self->block, self->block->bits);
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    madc_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t madc_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    madc_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(madc_init_obj, 1, madc_init);

STATIC mp_obj_t madc_block(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->block);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_block_obj, madc_block);

STATIC mp_obj_t madc_read(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t raw = madcblock_read_helper(self->block, self->channel_id);
    return MP_OBJ_NEW_SMALL_INT(raw);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_obj, madc_read);

STATIC mp_obj_t madc_read_u16(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t raw = madcblock_read_helper(self->block, self->channel_id);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    mp_int_t bits = self->block->bits;
    mp_uint_t u16 = raw << (16 - bits) | raw >> (2 * bits - 16);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_u16_obj, madc_read_u16);

STATIC mp_obj_t madc_read_uv(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    adc_atten_t atten = madc_atten_get(self);
    return MP_OBJ_NEW_SMALL_INT(madcblock_read_uv_helper(self->block, self->channel_id, atten));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_uv_obj, madc_read_uv);

STATIC mp_obj_t madc_atten(mp_obj_t self_in, mp_obj_t atten_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t atten = mp_obj_get_int(atten_in);
    madc_atten_helper(self, atten);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_atten_obj, madc_atten);

STATIC mp_obj_t madc_width(mp_obj_t self_in, mp_obj_t bits_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t bits = mp_obj_get_int(bits_in);
    madcblock_bits_helper(self->block, bits);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_width_obj, madc_width);

STATIC const mp_rom_map_elem_t madc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&madc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_block), MP_ROM_PTR(&madc_block_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&madc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&madc_read_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_uv), MP_ROM_PTR(&madc_read_uv_obj) },

    // Legacy API methods:
    { MP_ROM_QSTR(MP_QSTR_atten), MP_ROM_PTR(&madc_atten_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&madc_width_obj) },

    { MP_ROM_QSTR(MP_QSTR_ATTN_0DB), MP_ROM_INT(ADC_ATTEN_DB_0) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_2_5DB), MP_ROM_INT(ADC_ATTEN_DB_2_5) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_6DB), MP_ROM_INT(ADC_ATTEN_DB_6) },
    { MP_ROM_QSTR(MP_QSTR_ATTN_11DB), MP_ROM_INT(ADC_ATTEN_DB_11) },

    #if CONFIG_IDF_TARGET_ESP32
    { MP_ROM_QSTR(MP_QSTR_WIDTH_9BIT), MP_ROM_INT(9) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_10BIT), MP_ROM_INT(10) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_11BIT), MP_ROM_INT(11) },
    #endif
    #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    { MP_ROM_QSTR(MP_QSTR_WIDTH_12BIT), MP_ROM_INT(12) },
    #endif
    #if CONFIG_IDF_TARGET_ESP32S2
    { MP_ROM_QSTR(MP_QSTR_WIDTH_13BIT), MP_ROM_INT(13) },
    #endif

};
STATIC MP_DEFINE_CONST_DICT(madc_locals_dict, madc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = madc_print,
    .make_new = madc_make_new,
    .locals_dict = (mp_obj_t)&madc_locals_dict,
};
