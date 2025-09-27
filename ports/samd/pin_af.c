/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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
 *
 * This file provides and checks pin capabilities as required
 * for USART, I2C, SPI, PWM, ADC
 *
 */

#include <stdint.h>
#include "string.h"

#include "py/runtime.h"
#include "py/misc.h"
#include "extmod/modmachine.h"
#include "pin_af.h"
#include "sam.h"


extern const uint8_t tcc_channel_count[];

// Just look for an table entry for a given pin and raise an error
// in case of no match (which should not happen).
const machine_pin_obj_t *pin_find_by_id(int pin_id) {
    const mp_map_t *cpu_map = &machine_pin_cpu_pins_locals_dict.map;
    for (uint i = 0; i < cpu_map->alloc; i++) {
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR(cpu_map->table[i].value);
        if (pin->pin_id == pin_id) {
            return pin;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("not a Pin"));
}

static const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)&named_pins->map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL) {
        return named_elem->value;
    }
    return NULL;
}

const machine_pin_obj_t *pin_find(mp_obj_t pin) {
    // Is already a object of the proper type
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return pin;
    }
    if (mp_obj_is_small_int(pin)) {
        // Pin defined by pin number for PAnn, PBnn, etc.
        return pin_find_by_id(mp_obj_get_int(pin));
    }

    const machine_pin_obj_t *self = NULL;
    // See if the pin name matches a board pin
    self = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin);
    if (self != NULL) {
        return self;
    }
    // See if the pin name matches a cpu pin
    self = pin_find_named_pin(&machine_pin_cpu_pins_locals_dict, pin);
    if (self != NULL) {
        return self;
    }

    mp_raise_ValueError(MP_ERROR_TEXT("not a Pin"));
}

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_UART
// Test, whether the given pin is defined and has signals for sercom.
// If that applies return the alt_fct and pad_nr.
// If not, an error will be raised.

sercom_pad_config_t get_sercom_config(int pin_id, uint8_t sercom_nr) {
    const machine_pin_obj_t *pct_ptr = pin_find_by_id(pin_id);
    if ((pct_ptr->sercom1 >> 4) == sercom_nr) {
        return (sercom_pad_config_t) {ALT_FCT_SERCOM1, pct_ptr->sercom1 & 0x0f};
    } else if ((pct_ptr->sercom2 >> 4) == sercom_nr) {
        return (sercom_pad_config_t) {ALT_FCT_SERCOM2, pct_ptr->sercom2 & 0x0f};
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("wrong serial device"));
    }
}
#endif

#if MICROPY_PY_MACHINE_ADC
// Test, whether the given pin is defined as ADC.
// If that applies return the adc instance and channel.
// If not, an error will be raised.

adc_config_t get_adc_config(int pin_id, int32_t flag) {
    const machine_pin_obj_t *pct_ptr = pin_find_by_id(pin_id);
    if (pct_ptr->adc0 != 0xff && (flag & (1 << pct_ptr->adc0)) == 0) {
        return (adc_config_t) {0, pct_ptr->adc0};
    #if defined(MCU_SAMD51)
    } else if (pct_ptr->adc1 != 0xff && (flag & (1 << (pct_ptr->adc1 + 16))) == 0) {
        return (adc_config_t) {1, pct_ptr->adc1};
    #endif
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("ADC pin used"));
    }
}
#endif

#if MICROPY_PY_MACHINE_PWM

// Test, whether the given pin is defined and has signals for pwm.
// If that applies return the alt_fct, tcc number and channel number.
// If not, an error will be raised.
// The function either supplies a channel from a wanted device, or
// tries to provide an unused device, if available.

pwm_config_t get_pwm_config(int pin_id, int wanted_dev, uint8_t device_status[]) {
    const machine_pin_obj_t *pct_ptr = pin_find_by_id(pin_id);
    uint8_t tcc1 = pct_ptr->tcc1;
    uint8_t tcc2 = pct_ptr->tcc2;

    if (wanted_dev != -1) {
        if ((tcc1 >> 4) == wanted_dev) {
            return (pwm_config_t) {ALT_FCT_TCC1, tcc1};
        } else if ((tcc2 >> 4) == wanted_dev) {
            return (pwm_config_t) {ALT_FCT_TCC2, tcc2};
        }
    } else {
        // Try to get a unused PWM device at the pin
        if (((tcc1 >> 4) < TCC_INST_NUM) && (device_status[tcc1 >> 4] == 0)) {
            return (pwm_config_t) {ALT_FCT_TCC1, tcc1};
        }
        if (((tcc2 >> 4) < TCC_INST_NUM) && (device_status[tcc2 >> 4] == 0)) {
            return (pwm_config_t) {ALT_FCT_TCC2, tcc2};
        }
        // If all devices are used, return one from the pin if available
        if ((tcc1 >> 4) < TCC_INST_NUM) {
            return (pwm_config_t) {ALT_FCT_TCC1, tcc1};
        }
        if ((tcc2 >> 4) < TCC_INST_NUM) {
            return (pwm_config_t) {ALT_FCT_TCC2, tcc2};
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("not a PWM Pin"));
}

#endif

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET

// Configure a I2C pin. Used by machine_i2c.c and machine_i2c_target.c.
uint8_t pin_config_for_i2c(mp_obj_t pin_obj, uint8_t id, uint8_t pad_nr) {
    uint8_t pin = mp_hal_get_pin_obj(pin_obj);
    sercom_pad_config_t pad_config = get_sercom_config(pin, id);
    if (pad_config.pad_nr != pad_nr) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid sda/scl pin"));
    }
    // Configure the Pin mux.
    mp_hal_set_pin_mux(pin, pad_config.alt_fct);
    return pin;
}

#endif
