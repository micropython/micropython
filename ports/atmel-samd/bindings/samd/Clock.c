/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#include "bindings/samd/Clock.h"
#include "samd/clocks.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| .. currentmodule:: samd
//|
//| :class:`Clock` --- Clock reference
//| ------------------------------------------
//|
//| Identifies a clock on the microcontroller.
//|
//| .. class:: Clock
//|
//|    Identifies a clock on the microcontroller. They are fixed by the
//|    hardware so they cannot be constructed on demand. Instead, use
//|    `samd.clock` to reference the desired clock.
//|

STATIC void samd_clock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "%q.%q.%q", MP_QSTR_samd, MP_QSTR_clock, self->name);
}

//|     .. attribute:: enabled
//|
//|       Is the clock enabled? (read-only)
//|
STATIC mp_obj_t samd_clock_get_enabled(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(clock_get_enabled(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_enabled_obj, samd_clock_get_enabled);

const mp_obj_property_t samd_clock_enabled_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_enabled_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj,
    },
};

//|     .. attribute:: parent
//|
//|       Clock parent. (read-only)
//|
STATIC mp_obj_t samd_clock_get_parent(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t p_type, p_index;
    if (!clock_get_parent(self->type, self->index, &p_type, &p_index))
        return mp_const_none;

    const mp_map_t* samd_map = &samd_clock_globals.map;
    for (uint8_t i = 0; i < samd_map->alloc; i++) {
        samd_clock_obj_t *iter = samd_map->table[i].value;
        if (iter->type == p_type && iter->index == p_index)
            return iter;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_parent_obj, samd_clock_get_parent);

const mp_obj_property_t samd_clock_parent_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_parent_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj,
    },
};

//|     .. attribute:: frequency
//|
//|       Clock frequency. (read-only)
//|
STATIC mp_obj_t samd_clock_get_frequency(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(clock_get_frequency(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_frequency_obj, samd_clock_get_frequency);

const mp_obj_property_t samd_clock_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_frequency_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj,
    },
};

//|     .. attribute:: calibration
//|
//|       Clock calibration. Not all clocks can be calibrated.
//|
STATIC mp_obj_t samd_clock_get_calibration(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(clock_get_calibration(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_calibration_obj, samd_clock_get_calibration);

STATIC mp_obj_t samd_clock_set_calibration(mp_obj_t self_in, mp_obj_t calibration) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = clock_set_calibration(self->type, self->index, mp_obj_get_int(calibration));
    if (ret == -2)
        mp_raise_AttributeError(translate("calibration is read only"));
    if (ret == -1)
        mp_raise_ValueError(translate("calibration is out of range"));
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(samd_clock_set_calibration_obj, samd_clock_set_calibration);

const mp_obj_property_t samd_clock_calibration_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_calibration_obj,
              (mp_obj_t)&samd_clock_set_calibration_obj,
              (mp_obj_t)&mp_const_none_obj,
    },
};

STATIC const mp_rom_map_elem_t samd_clock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled), MP_ROM_PTR(&samd_clock_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_parent), MP_ROM_PTR(&samd_clock_parent_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&samd_clock_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&samd_clock_calibration_obj) },
};

STATIC MP_DEFINE_CONST_DICT(samd_clock_locals_dict, samd_clock_locals_dict_table);

const mp_obj_type_t samd_clock_type = {
    { &mp_type_type },
    .name = MP_QSTR_Clock,
    .print = samd_clock_print,
    .locals_dict = (mp_obj_t)&samd_clock_locals_dict,
};
