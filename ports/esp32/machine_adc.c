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
#include "esp_adc_cal.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"


#define DEFAULT_VREF 1100


typedef struct _madcblock_obj_t {
    mp_obj_base_t base;
    adc_unit_t unit_id;
    mp_int_t bits;
    adc_bits_width_t width;
    esp_adc_cal_characteristics_t *characteristics[ADC_ATTEN_MAX];
} madcblock_obj_t;

STATIC madcblock_obj_t madcblock_obj[] = {
    #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3
    {{&machine_adcblock_type}, ADC_UNIT_1, 12, -1, {0}},
    {{&machine_adcblock_type}, ADC_UNIT_2, 12, -1, {0}},
    #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    {{&machine_adcblock_type}, ADC_UNIT_1, 13, -1, {0}},
    {{&machine_adcblock_type}, ADC_UNIT_2, 13, -1, {0}},
    #endif
};

#define ADCBLOCK1 (&madcblock_obj[0])
#define ADCBLOCK2 (&madcblock_obj[1])


typedef struct _madc_obj_t {
    mp_obj_base_t base;
    madcblock_obj_t *block;
    adc_channel_t channel_id;
    gpio_num_t gpio_id;
} madc_obj_t;

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
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_0, GPIO_NUM_0},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_1, GPIO_NUM_2},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_2, GPIO_NUM_4},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_3, GPIO_NUM_12},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_4, GPIO_NUM_13},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_5, GPIO_NUM_14},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_6, GPIO_NUM_15},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_7, GPIO_NUM_25},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_8, GPIO_NUM_26},
    {{&machine_adc_type}, ADCBLOCK2, ADC_CHANNEL_9, GPIO_NUM_27},
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

STATIC adc_atten_t madc_obj_atten[MP_ARRAY_SIZE(madc_obj)];


STATIC void madcblock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    madcblock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADCBlock(%u, bits=%u)", self->unit_id, self->bits);
}


STATIC void madcblock_bits_helper(madcblock_obj_t *self, mp_int_t bits) {
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


STATIC void madc_init_helper(const madc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

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

    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        const madc_obj_t *adc = &madc_obj[i];
        if (self == adc->block && (channel_id == -1 || channel_id == adc->channel_id) && (gpio_id == -1 || gpio_id == adc->gpio_id)) {
            madc_init_helper(adc, 0, pos_args + n_pos_args, kw_args);
            return MP_OBJ_FROM_PTR(adc);
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("no matching ADC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(madcblock_connect_obj, 2, madcblock_connect);


STATIC uint32_t madcblock_read_helper(madcblock_obj_t *self, adc_channel_t channel_id) {
    int raw;
    if (self->unit_id == ADC_UNIT_1) {
        raw = adc1_get_raw(channel_id);
    } else {
        check_esp_err(adc2_get_raw(channel_id, self->width, &raw));
    }
    return (uint32_t)raw;
}


STATIC const mp_rom_map_elem_t madcblock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&madcblock_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&madcblock_connect_obj) },
};

STATIC MP_DEFINE_CONST_DICT(madcblock_locals_dict, madcblock_locals_dict_table);

const mp_obj_type_t machine_adcblock_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADCBlock,
    .print = madcblock_print,
    .make_new = madcblock_make_new,
    .locals_dict = (mp_obj_t)&madcblock_locals_dict,
};


STATIC void madc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC(Pin(%u), atten=%u)", self->gpio_id, madc_obj_atten[self - madc_obj]);
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
    madc_obj_atten[self - madc_obj] = atten;
}


STATIC void madc_init_helper(const madc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    } else if (madc_obj_atten[self - madc_obj] == -1) {
        madc_atten_helper(self, ADC_ATTEN_DB_0);
    }
}


STATIC mp_obj_t madc_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    gpio_num_t gpio_id = machine_pin_get_id(args[0]);
    const madc_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        if (gpio_id == madc_obj[i].gpio_id) {
            self = &madc_obj[i];
            break;
        }
    }
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
    uint32_t raw = madcblock_read_helper(self->block, self->channel_id);
    return MP_OBJ_NEW_SMALL_INT(raw);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_obj, madc_read);


STATIC mp_obj_t madc_read_u16(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t raw = madcblock_read_helper(self->block, self->channel_id);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    mp_int_t bits = self->block->bits;
    uint32_t u16 = raw << (16 - bits) | raw >> (2 * bits - 16);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_u16_obj, madc_read_u16);


STATIC mp_obj_t madc_read_uv(mp_obj_t self_in) {
    const madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t raw = madcblock_read_helper(self->block, self->channel_id);
    adc_atten_t atten = madc_obj_atten[self - madc_obj];
    esp_adc_cal_characteristics_t *adc_chars = self->block->characteristics[atten];
    if (adc_chars == NULL) {
        adc_chars = malloc(sizeof(esp_adc_cal_characteristics_t));
        esp_adc_cal_characterize(self->block->unit_id, atten, self->block->width, DEFAULT_VREF, adc_chars);
        self->block->characteristics[atten] = adc_chars;
    }
    mp_int_t uv = esp_adc_cal_raw_to_voltage(raw, adc_chars) * 1000;
    return MP_OBJ_NEW_SMALL_INT(uv);
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
