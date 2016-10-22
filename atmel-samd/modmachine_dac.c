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
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "modmachine_dac.h"

#include "asf/sam0/drivers/dac/dac.h"

/// \moduleref machine
/// \class DAC - digital to analog conversion
///
/// The DAC is used to output analog values (a specific voltage) on pin PA02
/// (Arduino Zero A0, Feather MO Bluefruit A0).
/// The voltage will be between 0 and 3.3V.
///
/// *This module will undergo changes to the API.*
///
/// Example usage:
///
///     from machine import DAC
///
///     dac = DAC()             # create DAC 1 on pin PA02
///     dac.write(512)          # write a value to the DAC (makes PA02 1.65V)
///     dac.write_mv(1650)      # Also results in PA02 being 1.65V

/******************************************************************************/
// Micro Python bindings

typedef struct _dac_obj_t {
    mp_obj_base_t base;
    struct dac_module dac_instance;
} dac_obj_t;

// create the dac object

/// \classmethod \constructor()
/// Construct a new DAC object for pin PA02.
STATIC mp_obj_t dac_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    dac_obj_t *dac = m_new_obj(dac_obj_t);
    dac->base.type = &dac_type;

    struct dac_config config_dac;
    dac_get_config_defaults(&config_dac);
    config_dac.reference = DAC_REFERENCE_AVCC;
    enum status_code status = dac_init(&dac->dac_instance, DAC, &config_dac);
    if (status != STATUS_OK) {
      // Throw error.
      return mp_const_none;
    }

    struct dac_chan_config config_dac_chan;
    dac_chan_get_config_defaults(&config_dac_chan);
    dac_chan_set_config(&dac->dac_instance, DAC_CHANNEL_0, &config_dac_chan);
    dac_chan_enable(&dac->dac_instance, DAC_CHANNEL_0);

    dac_enable(&dac->dac_instance);

    // return object
    return dac;
}

/// \method deinit()
/// Turn off the DAC, enable other use of pin.
STATIC mp_obj_t dac_del(mp_obj_t self_in) {
    dac_obj_t *self = self_in;

    dac_disable(&self->dac_instance);
    dac_chan_disable(&self->dac_instance, DAC_CHANNEL_0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dac_del_obj, dac_del);

/// \method write(value)
/// Direct access to the DAC output (10 bit).
STATIC mp_obj_t dac_write(mp_obj_t self_in, mp_obj_t val) {
    dac_obj_t *self = self_in;

    dac_chan_write(&self->dac_instance, DAC_CHANNEL_0, mp_obj_get_int(val));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(dac_write_obj, dac_write);

/// \method write_mv(value)
/// Direct access to the DAC output by specifying millivolts.
STATIC mp_obj_t dac_write_mv(mp_obj_t self_in, mp_obj_t val_mv) {
    dac_obj_t *self = self_in;
    // TODO(tannewt): Sanity check that the mv value is less than the reference
    // voltage.
    uint16_t val = ((uint32_t) mp_obj_get_int(val_mv)) * 1023 / 3300;

    dac_chan_write(&self->dac_instance, DAC_CHANNEL_0, val);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(dac_write_mv_obj, dac_write_mv);

STATIC const mp_map_elem_t dac_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&dac_del_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&dac_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_mv), (mp_obj_t)&dac_write_mv_obj },
};

STATIC MP_DEFINE_CONST_DICT(dac_locals_dict, dac_locals_dict_table);

const mp_obj_type_t dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .make_new = dac_make_new,
    .locals_dict = (mp_obj_t)&dac_locals_dict,
};
