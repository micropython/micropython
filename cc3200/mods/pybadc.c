/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/gc.h"
#include "bufhelper.h"
#include "inc/hw_types.h"
#include "inc/hw_adc.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "interrupt.h"
#include "pin.h"
#include "prcm.h"
#include "adc.h"
#include "pybadc.h"
#include "pybpin.h"
#include "pins.h"
#include "mpexception.h"


/// \moduleref pyb
/// \class ADC - analog to digital conversion: read analog values on a pin
///
/// Usage:
///
///     adc = pyb.ADC(channel)                # create an adc object on the given channel (0 to 3)
///                                             this automatically configures the pin associated to
///                                             that analog channel.
///     adc.read()                            # read channel value
///
///     The sample rate is fixed to 62.5KHz and the resolution to 12 bits.

typedef struct _pyb_obj_adc_t {
    mp_obj_base_t base;
    byte channel;
    byte num;
} pyb_obj_adc_t;


/******************************************************************************/
/* Micro Python bindings : adc object                                         */

STATIC void adc_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_obj_adc_t *self = self_in;
    print(env, "<ADC, channel=%u>", self->num);
}

/// \classmethod \constructor(channel)
/// Create an ADC object associated with the given channel.
/// This allows you to then read analog values on that pin.
STATIC mp_obj_t adc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // the first argument is the channel number
    uint num = mp_obj_get_int(args[0]);
    const pin_obj_t *pin;
    uint channel;
    switch (num) {
    case 0:
        channel = ADC_CH_0;
        pin = &pin_GPIO2;
        break;
    case 1:
        channel = ADC_CH_1;
        pin = &pin_GPIO3;
        break;
    case 2:
        channel = ADC_CH_2;
        pin = &pin_GPIO4;
        break;
    case 3:
        channel = ADC_CH_3;
        pin = &pin_GPIO5;
        break;
    default:
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        break;
    }

    // disable the callback before re-configuring
    pyb_obj_adc_t *self = m_new_obj_with_finaliser(pyb_obj_adc_t);
    self->base.type = &pyb_adc_type;
    self->channel = channel;
    self->num = num;

    // configure the pin in analog mode
    pin_config (pin, 0, 0, PIN_TYPE_ANALOG, PIN_STRENGTH_2MA);

    // enable the ADC channel
    MAP_ADCChannelEnable(ADC_BASE, channel);
    // enable and configure the timer
    MAP_ADCTimerConfig(ADC_BASE, (1 << 17) - 1);
    MAP_ADCTimerEnable(ADC_BASE);
    // enable the ADC peripheral
    MAP_ADCEnable(ADC_BASE);

    return self;
}

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
STATIC mp_obj_t adc_read(mp_obj_t self_in) {
    pyb_obj_adc_t *self = self_in;
    uint32_t sample;

    // wait until a new value is available
    while (!MAP_ADCFIFOLvlGet(ADC_BASE, self->channel));
    // read the sample
    sample = MAP_ADCFIFORead(ADC_BASE, self->channel);
    // the 12 bit sampled value is stored in bits [13:2]
    return MP_OBJ_NEW_SMALL_INT((sample & 0x3FFF) >> 2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

/// \method enable()
/// Enable the adc channel
STATIC mp_obj_t adc_enable(mp_obj_t self_in) {
    pyb_obj_adc_t *self = self_in;

    MAP_ADCChannelEnable(ADC_BASE, self->channel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_enable_obj, adc_enable);

/// \method disable()
/// Disable the adc channel
STATIC mp_obj_t adc_disable(mp_obj_t self_in) {
    pyb_obj_adc_t *self = self_in;

    MAP_ADCChannelDisable(ADC_BASE, self->channel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_disable_obj, adc_disable);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__),             (mp_obj_t)&adc_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),                (mp_obj_t)&adc_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),              (mp_obj_t)&adc_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),             (mp_obj_t)&adc_disable_obj },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};

