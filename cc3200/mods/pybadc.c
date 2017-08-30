/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/gc.h"
#include "py/mperrno.h"
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


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYB_ADC_NUM_CHANNELS                4

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
    bool enabled;
} pyb_adc_obj_t;

typedef struct {
    mp_obj_base_t base;
    pin_obj_t *pin;
    byte channel;
    byte id;
    bool enabled;
} pyb_adc_channel_obj_t;


/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_adc_channel_obj_t pyb_adc_channel_obj[PYB_ADC_NUM_CHANNELS] = { {.pin = &pin_GP2, .channel = ADC_CH_0, .id = 0, .enabled = false},
                                                                           {.pin = &pin_GP3, .channel = ADC_CH_1, .id = 1, .enabled = false},
                                                                           {.pin = &pin_GP4, .channel = ADC_CH_2, .id = 2, .enabled = false},
                                                                           {.pin = &pin_GP5, .channel = ADC_CH_3, .id = 3, .enabled = false} };
STATIC pyb_adc_obj_t pyb_adc_obj = {.enabled = false};

STATIC const mp_obj_type_t pyb_adc_channel_type;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC mp_obj_t adc_channel_deinit(mp_obj_t self_in);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
STATIC void pyb_adc_init (pyb_adc_obj_t *self) {
    // enable and configure the timer
    MAP_ADCTimerConfig(ADC_BASE, (1 << 17) - 1);
    MAP_ADCTimerEnable(ADC_BASE);
    // enable the ADC peripheral
    MAP_ADCEnable(ADC_BASE);
    self->enabled = true;
}

STATIC void pyb_adc_check_init(void) {
    // not initialized
    if (!pyb_adc_obj.enabled) {
        mp_raise_OSError(MP_EPERM);
    }
}

STATIC void pyb_adc_channel_init (pyb_adc_channel_obj_t *self) {
    // the ADC block must be enabled first
    pyb_adc_check_init();
    // configure the pin in analog mode
    pin_config (self->pin, -1, PIN_TYPE_ANALOG, PIN_TYPE_STD, -1, PIN_STRENGTH_2MA);
    // enable the ADC channel
    MAP_ADCChannelEnable(ADC_BASE, self->channel);
    self->enabled = true;
}

STATIC void pyb_adc_deinit_all_channels (void) {
    for (int i = 0; i < PYB_ADC_NUM_CHANNELS; i++) {
        adc_channel_deinit(&pyb_adc_channel_obj[i]);
    }
}

/******************************************************************************/
/* MicroPython bindings : adc object                                         */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_adc_obj_t *self = self_in;
    if (self->enabled) {
        mp_printf(print, "ADC(0, bits=12)");
    } else {
        mp_printf(print, "ADC(0)");
    }
}

STATIC const mp_arg_t pyb_adc_init_args[] = {
    { MP_QSTR_id,                          MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_bits,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 12} },
};
STATIC mp_obj_t adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_adc_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_adc_init_args, args);

    // check the peripheral id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // check the number of bits
    if (args[1].u_int != 12) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }

    // setup the object
    pyb_adc_obj_t *self = &pyb_adc_obj;
    self->base.type = &pyb_adc_type;

    // initialize and register with the sleep module
    pyb_adc_init(self);
    pyb_sleep_add ((const mp_obj_t)self, (WakeUpCB_t)pyb_adc_init);
    return self;
}

STATIC mp_obj_t adc_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_adc_init_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &pyb_adc_init_args[1], args);
    // check the number of bits
    if (args[0].u_int != 12) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
    pyb_adc_init(pos_args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(adc_init_obj, 1, adc_init);

STATIC mp_obj_t adc_deinit(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;
    // first deinit all channels
    pyb_adc_deinit_all_channels();
    MAP_ADCDisable(ADC_BASE);
    self->enabled = false;
    // unregister it with the sleep module
    pyb_sleep_remove ((const mp_obj_t)self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_deinit_obj, adc_deinit);

STATIC mp_obj_t adc_channel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_adc_channel_args[] = {
        { MP_QSTR_id,                          MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pin,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_adc_channel_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_adc_channel_args, args);

    uint ch_id;
    if (args[0].u_obj != MP_OBJ_NULL) {
        ch_id = mp_obj_get_int(args[0].u_obj);
        if (ch_id >= PYB_ADC_NUM_CHANNELS) {
            mp_raise_ValueError(mpexception_value_invalid_arguments);
        } else if (args[1].u_obj != mp_const_none) {
            uint pin_ch_id = pin_find_peripheral_type (args[1].u_obj, PIN_FN_ADC, 0);
            if (ch_id != pin_ch_id) {
                mp_raise_ValueError(mpexception_value_invalid_arguments);
            }
        }
    } else {
        ch_id = pin_find_peripheral_type (args[1].u_obj, PIN_FN_ADC, 0);
    }

    // setup the object
    pyb_adc_channel_obj_t *self = &pyb_adc_channel_obj[ch_id];
    self->base.type = &pyb_adc_channel_type;
    pyb_adc_channel_init (self);
    // register it with the sleep module
    pyb_sleep_add ((const mp_obj_t)self, (WakeUpCB_t)pyb_adc_channel_init);
    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(adc_channel_obj, 1, adc_channel);

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&adc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&adc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel),             MP_ROM_PTR(&adc_channel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};

STATIC void adc_channel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_adc_channel_obj_t *self = self_in;
    if (self->enabled) {
        mp_printf(print, "ADCChannel(%u, pin=%q)", self->id, self->pin->name);
    } else {
        mp_printf(print, "ADCChannel(%u)", self->id);
    }
}

STATIC mp_obj_t adc_channel_init(mp_obj_t self_in) {
    pyb_adc_channel_obj_t *self = self_in;
    // re-enable it
    pyb_adc_channel_init(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_channel_init_obj, adc_channel_init);

STATIC mp_obj_t adc_channel_deinit(mp_obj_t self_in) {
    pyb_adc_channel_obj_t *self = self_in;

    MAP_ADCChannelDisable(ADC_BASE, self->channel);
    // unregister it with the sleep module
    pyb_sleep_remove ((const mp_obj_t)self);
    self->enabled = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_channel_deinit_obj, adc_channel_deinit);

STATIC mp_obj_t adc_channel_value(mp_obj_t self_in) {
    pyb_adc_channel_obj_t *self = self_in;
    uint32_t value;

    // the channel must be enabled
    if (!self->enabled) {
        mp_raise_OSError(MP_EPERM);
    }

    // wait until a new value is available
    while (!MAP_ADCFIFOLvlGet(ADC_BASE, self->channel));
    // read the sample
    value = MAP_ADCFIFORead(ADC_BASE, self->channel);
    // the 12 bit sampled value is stored in bits [13:2]
    return MP_OBJ_NEW_SMALL_INT((value & 0x3FFF) >> 2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_channel_value_obj, adc_channel_value);

STATIC mp_obj_t adc_channel_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return adc_channel_value (self_in);
}

STATIC const mp_rom_map_elem_t adc_channel_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&adc_channel_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&adc_channel_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),               MP_ROM_PTR(&adc_channel_value_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_channel_locals_dict, adc_channel_locals_dict_table);

STATIC const mp_obj_type_t pyb_adc_channel_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADCChannel,
    .print = adc_channel_print,
    .call = adc_channel_call,
    .locals_dict = (mp_obj_t)&adc_channel_locals_dict,
};
