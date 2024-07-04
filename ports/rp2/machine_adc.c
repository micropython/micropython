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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#include "py/mphal.h"
#include "hardware/adc.h"
#include "machine_pin.h"

#define ADC_IS_VALID_GPIO(gpio) ((gpio) >= 26 && (gpio) <= 29)
#define ADC_CHANNEL_FROM_GPIO(gpio) ((gpio) - 26)
#define ADC_CHANNEL_TEMPSENSOR (4)

static uint16_t adc_config_and_read_u16(uint32_t channel) {
    adc_select_input(channel);
    uint32_t raw = adc_read();
    const uint32_t bits = 12;
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    return raw << (16 - bits) | raw >> (2 * bits - 16);
}

/******************************************************************************/
// MicroPython bindings for machine.ADC

#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(ADC_CHANNEL_TEMPSENSOR) }, \

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint32_t channel;
    #if MICROPY_HW_ADC_EXT_COUNT
    uint32_t is_ext : 1;
    #endif
} machine_adc_obj_t;

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ADC channel=%u>", self->channel);
}

// ADC(id)
static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];

    uint32_t channel = -1;
    bool is_ext = false;
    const machine_pin_obj_t *pin = NULL;

    if (mp_obj_is_int(source)) {
        channel = mp_obj_get_int(source);
        if (!(channel >= 0 && channel <= ADC_CHANNEL_TEMPSENSOR)) {
            // Not a valid ADC channel, fallback to searching for a pin.
            channel = -1;
        }
    }

    if (channel == -1) {
        // Get GPIO and check it has ADC capabilities.
        pin = machine_pin_find(source);
        bool valid_adc_pin = false;
        #if MICROPY_HW_ADC_EXT_COUNT
        is_ext = pin->is_ext;
        if (is_ext) {
            valid_adc_pin = machine_pin_ext_is_adc_channel(pin);
        } else
        #endif
        {
            valid_adc_pin = ADC_IS_VALID_GPIO(pin->id);
        }
        if (!valid_adc_pin) {
            mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't have ADC capabilities"));
        }
    }

    // Initialise the ADC peripheral if it's not already running.
    if (!is_ext && !(adc_hw->cs & ADC_CS_EN_BITS)) {
        adc_init();
    }

    if (pin) {
        if (is_ext) {
            #if MICROPY_HW_ADC_EXT_COUNT
            // Note external pins are mutable.
            machine_pin_ext_config((machine_pin_obj_t *)pin, MACHINE_PIN_MODE_ANALOG, 0);
            channel = machine_pin_ext_to_adc_channel(pin);
            #endif
        } else {
            // Configure the GPIO pin in ADC mode.
            adc_gpio_init(pin->id);
            channel = ADC_CHANNEL_FROM_GPIO(pin->id);
        }
    } else if (channel == ADC_CHANNEL_TEMPSENSOR) {
        // Enable temperature sensor.
        adc_set_temp_sensor_enabled(1);
    }

    // Create ADC object.
    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->channel = channel;
    #if MICROPY_HW_ADC_EXT_COUNT
    o->is_ext = is_ext;
    #endif

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    #if MICROPY_HW_ADC_EXT_COUNT
    if (self->is_ext) {
        return machine_pin_ext_read_u16(self->channel);
    }
    #endif
    return adc_config_and_read_u16(self->channel);
}
