/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "fsl_common.h"
#include "fsl_common_arm.h"

/* Note: the HAL pin interface is used by several extmods,
 * which requires a machine.Pin object as its parameter.
 * As the corresponding PORT and GPIO clocks are initialized in drv_pin_init(),
 * which is called by Machine.Pin() constructor, only basic functions are required here.
 * Same for alternative functions and pin modes.
 */

const mp_hal_pin_obj_t mp_hal_pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;

    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);

    if (named_elem == NULL || named_elem->value == MP_OBJ_NULL) {
        return NULL;
    }

    return MP_OBJ_TO_PTR(named_elem->value);
}

const machine_pin_af_t *mp_hal_pin_find_af(const mp_hal_pin_obj_t pin, machine_pin_af_type_t type,
    uint8_t idx, machine_pin_af_attr_t attr) {
    for (uint8_t i = 0; i < pin->af_count; i++) {
        if (pin->af[i].af_type != type) {
            continue;
        }

        if (pin->af[i].af_instance_id != idx) {
            continue;
        }

        if (pin->af[i].af_attribute != attr) {
            continue;
        }

        return &pin->af[i];
    }

    return NULL;
}

const machine_pin_adc_t *mp_hal_pin_find_adc(const mp_hal_pin_obj_t pin) {
    return pin->adc;
}

const machine_pin_af_t *mp_hal_pin_find_pwm(const mp_hal_pin_obj_t pin) {
    for (uint8_t i = 0; i < pin->af_count; i++) {
        if (pin->af[i].af_type != MACHINE_PIN_AF_TYPE_CT32) {
            continue;
        }

        /* Attribute with bit 7 set is a input capture channel */
        if (pin->af[i].af_attribute & 0x80) {
            continue;
        }

        return &pin->af[i];
    }

    return NULL;
}

bool mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    return drv_pin_read(&p);
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, bool value) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_write(&p, value);
}

void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_write(&p, true);
}

void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_write(&p, true);
}

void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_write(&p, false);
}

void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_write(&p, false);
}

void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_OUTPUT_OD, 0U);
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_OUTPUT_PP, 0U);
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_INPUT, 0U);
}

void mp_hal_pin_analog(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_ANALOG, 0U);
}

void mp_hal_pin_af(mp_hal_pin_obj_t pin, uint8_t af_num) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_AF_PP, af_num);
}

void mp_hal_pin_af_od(mp_hal_pin_obj_t pin, uint8_t af_num) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_mode_set(&p, DRV_PIN_MODE_AF_OD, af_num);
}

void mp_hal_pin_pull_up(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_pull_set(&p, DRV_PIN_PULL_UP);
}

void mp_hal_pin_pull_down(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_pull_set(&p, DRV_PIN_PULL_DOWN);
}

void mp_hal_pin_pull_none(mp_hal_pin_obj_t pin) {
    drv_pin_t p = {
        .port = pin->port,
        .pin = pin->pin,
    };

    drv_pin_pull_set(&p, DRV_PIN_PULL_NONE);
}
