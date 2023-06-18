/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "ra_adc.h"

#define ADC_SAMPLETIME_DEFAULT  1
#define ADC_CHANNEL_VREFINT     (ADC_REF)
#define ADC_CHANNEL_TEMPSENSOR  (ADC_TEMP)
#define ADC_SAMPLETIME_DEFAULT_INT  1

typedef struct {
    uint8_t dummy;
} ADC_TypeDef;

// Timeout for waiting for end-of-conversion
#define ADC_EOC_TIMEOUT_MS (10)

// This is a synthesised channel representing the maximum ADC reading (useful to scale other channels)
#define ADC_CHANNEL_VREF (0xffff)

/******************************************************************************/
// MicroPython bindings for machine.ADC

const mp_obj_type_t machine_adc_type;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_TypeDef *adc;
    uint32_t channel;
    uint32_t pin;
    uint32_t sample_time;
} machine_adc_obj_t;

STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t resolution = (uint8_t)ra_adc_get_resolution();
    mp_printf(print, "<ADC%u channel=%u>", resolution, self->channel);
}

// ADC(id)
STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];
    bool find = false;
    uint8_t channel;
    uint32_t pin;
    uint32_t sample_time = ADC_SAMPLETIME_DEFAULT;
    if (mp_obj_is_int(source)) {
        channel = (uint8_t)mp_obj_get_int(source);
        find = ra_adc_ch_to_pin((uint8_t)channel, (uint32_t *)&pin);
        if (!find) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Channel(%q) does not have ADC capabilities"), channel);
        }
    } else {
        const machine_pin_obj_t *pin_obj = machine_pin_find(source);
        find = ra_adc_pin_to_ch((uint32_t)pin_obj->pin, (uint8_t *)&channel);
        if (!find) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) does not have ADC capabilities"), pin_obj->name);
        }
        pin = pin_obj->pin;
    }
    ra_adc_init();
    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->adc = (ADC_TypeDef *)NULL;
    o->channel = channel;
    o->pin = pin;
    o->sample_time = sample_time;
    ra_adc_enable((uint8_t)pin);
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t machine_adc_read(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(ra_adc_read((uint32_t)(self->pin)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_obj, machine_adc_read);

STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    const machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t raw = (mp_uint_t)ra_adc_read((uint32_t)(self->pin));
    mp_int_t bits = (mp_int_t)ra_adc_get_resolution();
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    mp_uint_t u16 = raw << (16 - bits) | raw >> (2 * bits - 16);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_adc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },

    { MP_ROM_QSTR(MP_QSTR_VREF), MP_ROM_INT(ADC_CHANNEL_VREF) },
    { MP_ROM_QSTR(MP_QSTR_CORE_VREF), MP_ROM_INT(ADC_CHANNEL_VREFINT) },
    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(ADC_CHANNEL_TEMPSENSOR) },
    #if defined(ADC_CHANNEL_VBAT)
    { MP_ROM_QSTR(MP_QSTR_CORE_VBAT), MP_ROM_INT(ADC_CHANNEL_VBAT) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adc_make_new,
    locals_dict, &machine_adc_locals_dict,
    print, machine_adc_print
    );
