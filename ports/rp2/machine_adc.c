/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "hardware/adc.h"

#define ADC_IS_VALID_GPIO(gpio) ((gpio) >= 26 && (gpio) <= 29)
#define ADC_CHANNEL_FROM_GPIO(gpio) ((gpio) - 26)
#define ADC_CHANNEL_TEMPSENSOR (4)

STATIC uint16_t adc_config_and_read_u16(uint32_t channel) {
    adc_select_input(channel);
    uint32_t raw = adc_read();
    const uint32_t bits = 12;
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    return raw << (16 - bits) | raw >> (2 * bits - 16);
}

/******************************************************************************/
// MicroPython bindings for machine.ADC

const mp_obj_type_t machine_adc_type;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint32_t channel;
} machine_adc_obj_t;

STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ADC channel=%u>", self->channel);
}

// ADC(id)
STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];

    uint32_t channel;
    if (mp_obj_is_int(source)) {
        // Get and validate channel number.
        channel = mp_obj_get_int(source);
        if (!((channel >= 0 && channel <= ADC_CHANNEL_TEMPSENSOR) || ADC_IS_VALID_GPIO(channel))) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid channel"));
        }

    } else {
        // Get GPIO and check it has ADC capabilities.
        channel = mp_hal_get_pin_obj(source);
        if (!ADC_IS_VALID_GPIO(channel)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't have ADC capabilities"));
        }
    }

    // Initialise the ADC peripheral if it's not already running.
    if (!(adc_hw->cs & ADC_CS_EN_BITS)) {
        adc_init();
    }

    if (ADC_IS_VALID_GPIO(channel)) {
        // Configure the GPIO pin in ADC mode.
        adc_gpio_init(channel);
        channel = ADC_CHANNEL_FROM_GPIO(channel);
    } else if (channel == ADC_CHANNEL_TEMPSENSOR) {
        // Enable temperature sensor.
        adc_set_temp_sensor_enabled(1);
    }

    // Create ADC object.
    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->channel = channel;

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(adc_config_and_read_u16(self->channel));
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },

    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(ADC_CHANNEL_TEMPSENSOR) },
};
STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = machine_adc_print,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_adc_locals_dict,
};
