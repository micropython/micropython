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
#include "gpio.h"
#include "prcm.h"
#include "adc.h"
#include "pybadc.h"
#include "pybpin.h"
#include "pybsleep.h"
#include "pins.h"
#include "mpexception.h"


/// \moduleref pyb
/// \class ADC - analog to digital conversion: read analog values on a pin
///
/// Usage:
///
///     adc = pyb.ADC('GP5')                # create an adc object on the given pin (GP2, GP3, GP4 o GP5)
///     adc.read()                          # read channel value
///
///     The sample rate is fixed to 62.5KHz and the resolution to 12 bits.


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYB_ADC_NUM_CHANNELS                4

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
    pin_obj_t *pin;
    byte channel;
    byte id;
} pyb_adc_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_adc_obj_t pyb_adc_obj[PYB_ADC_NUM_CHANNELS] = { {.pin = &pin_GP2, .channel = ADC_CH_0, .id = 1}, {.pin = &pin_GP3, .channel = ADC_CH_1, .id = 2},
                                                           {.pin = &pin_GP4, .channel = ADC_CH_2, .id = 2}, {.pin = &pin_GP5, .channel = ADC_CH_3, .id = 4} };

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
STATIC void pybadc_init (pyb_adc_obj_t *self) {
    // configure the pin in analog mode
    pin_config (self->pin, PIN_MODE_0, GPIO_DIR_MODE_IN, PYBPIN_ANALOG_TYPE, PIN_STRENGTH_2MA);
    // enable the ADC channel
    MAP_ADCChannelEnable(ADC_BASE, self->channel);
    // enable and configure the timer
    MAP_ADCTimerConfig(ADC_BASE, (1 << 17) - 1);
    MAP_ADCTimerEnable(ADC_BASE);
    // enable the ADC peripheral
    MAP_ADCEnable(ADC_BASE);
}

/******************************************************************************/
/* Micro Python bindings : adc object                                         */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_adc_obj_t *self = self_in;
    mp_printf(print, "<ADC1 channel=%u on %q>", self->id, self->pin->name);
}

/// \classmethod \constructor(pin)
/// Create an ADC object associated with the given pin.
/// This allows you to then read analog values on that pin.
STATIC mp_obj_t adc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // the argument passed is the pin
    const pin_obj_t *pin = (pin_obj_t *)pin_find(args[0]);
    for (int32_t idx = 0; idx < PYB_ADC_NUM_CHANNELS; idx++) {
        if (pin == pyb_adc_obj[idx].pin) {
            pyb_adc_obj_t *self = &pyb_adc_obj[idx];
            self->base.type = &pyb_adc_type;
            pybadc_init (self);
            // register it with the sleep module
            pybsleep_add ((const mp_obj_t)self, (WakeUpCB_t)pybadc_init);
            return self;
        }
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
STATIC mp_obj_t adc_read(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;
    uint32_t sample;

    // wait until a new value is available
    while (!MAP_ADCFIFOLvlGet(ADC_BASE, self->channel));
    // read the sample
    sample = MAP_ADCFIFORead(ADC_BASE, self->channel);
    // the 12 bit sampled value is stored in bits [13:2]
    return MP_OBJ_NEW_SMALL_INT((sample & 0x3FFF) >> 2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

/// \method init()
/// Enable the adc channel
STATIC mp_obj_t adc_init(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;

    pybadc_init(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_init_obj, adc_init);

/// \method deinit()
/// Disable the adc channel
STATIC mp_obj_t adc_deinit(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;

    MAP_ADCChannelDisable(ADC_BASE, self->channel);
    // unregister it with the sleep module
    pybsleep_remove ((const mp_obj_t)self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_deinit_obj, adc_deinit);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),                (mp_obj_t)&adc_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&adc_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),              (mp_obj_t)&adc_deinit_obj },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};

