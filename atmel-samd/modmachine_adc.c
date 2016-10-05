/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "modmachine_adc.h"
#include "modmachine_pin.h"

#include "asf/sam0/drivers/adc/adc.h"

/// \moduleref machine
/// \class ADC - analog to digital conversion: read analog values on a pin
///
/// Usage:
///
///     adc = machine.ADC(pin)              # create an analog object from a pin
///     val = adc.read()                    # read an analog value

typedef struct {
    mp_obj_base_t base;
    pin_obj_t * pin;
    struct adc_module adc_instance;
} adc_obj_t;

/******************************************************************************/
/* Micro Python bindings : adc object (single channel)                        */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    adc_obj_t *self = self_in;
    mp_print_str(print, "<ADC on ");
    mp_obj_print_helper(print, self->pin, PRINT_STR);
}


STATIC void adc_init_single(adc_obj_t *adc_obj) {
  struct adc_config config_adc;
  adc_get_config_defaults(&config_adc);

  config_adc.positive_input = adc_obj->pin->adc_input;
  config_adc.resolution = ADC_RESOLUTION_CUSTOM;
  config_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_16;
  config_adc.divide_result = ADC_DIVIDE_RESULT_16;
  config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV128;

  adc_init(&adc_obj->adc_instance, ADC, &config_adc);
}

/// \classmethod \constructor(pin)
/// Create an ADC object associated with the given pin.
/// This allows you to then read analog values on that pin.
STATIC mp_obj_t adc_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin name
    mp_obj_t pin_obj = args[0];

    const pin_obj_t *pin = pin_find(pin_obj);
    if (!pin->has_adc) {
        // No ADC function on that pin
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %q does not have ADC capabilities", pin->name));
    }

    adc_obj_t *o = m_new_obj(adc_obj_t);
    memset(o, 0, sizeof(*o));
    o->base.type = &adc_type;
    o->pin = pin_obj;
    adc_init_single(o);

    return o;
}

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
// TODO(tannewt): Don't turn it all on just for one read. This simplifies
// handling of reading multiple inputs and surviving sleep though so for now its
// ok.
STATIC mp_obj_t mp_adc_read(mp_obj_t self_in) {
    adc_obj_t *self = self_in;

    adc_enable(&self->adc_instance);
    adc_start_conversion(&self->adc_instance);

    uint16_t data;
    enum status_code status = adc_read(&self->adc_instance, &data);
    while (status == STATUS_BUSY) {
      status = adc_read(&self->adc_instance, &data);
    }
    if (status == STATUS_ERR_OVERFLOW) {
      // TODO(tannewt): Throw an error.
    }

    adc_disable(&self->adc_instance);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, mp_adc_read);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&adc_read_obj},
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};
