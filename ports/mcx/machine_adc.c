/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.


#include "py/mphal.h"

#include "drv_adc.h"

#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    drv_adc_t drv;
    drv_adc_side_t side;
    uint8_t channel;
} machine_adc_obj_t;

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "ADC(%u,", self->id);

    char side = 'A';
    if (self->side == DRV_ADCSideB) {
        side = 'B';
    }

    mp_printf(print, " side=%c, channel=%u)", side, self->channel);
}

static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t pin_obj = args[0];
    if (!mp_obj_is_type(pin_obj, &machine_pin_type)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Object is not a Pin object."));
    }

    mp_hal_pin_obj_t pin = MP_OBJ_TO_PTR(pin_obj);
    const machine_pin_adc_t *adc = mp_hal_pin_find_adc(pin);
    if (adc == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin does not have ADC capabilities."));
    }

    machine_adc_obj_t *self = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);

    mp_hal_pin_analog(pin);

    self->id = adc->adc_id;
    self->channel = adc->adc_channel;

    switch (adc->adc_side) {
        case 'A':
        default:
            self->side = DRV_ADCSideA;
            break;

        case 'B':
            self->side = DRV_ADCSideB;
            break;
    }

    if (drv_adc_init(&self->drv, self->id) < 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to initialize ADC"));
    }

    return MP_OBJ_FROM_PTR(self);

}

static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    uint16_t ad_val;
    int ret = drv_adc_measure(&self->drv, self->side, self->channel, &ad_val);
    if (ret < 0) {
        return ret;
    }

    return ad_val;
}
