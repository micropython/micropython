/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#if MICROPY_PY_MACHINE_ADC

#include "extmod/modmachine.h"

// The port must provide implementations of these low-level ADC functions.

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self);

#if MICROPY_PY_MACHINE_ADC_INIT
static void mp_machine_adc_init_helper(machine_adc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
#endif

#if MICROPY_PY_MACHINE_ADC_DEINIT
static void mp_machine_adc_deinit(machine_adc_obj_t *self);
#endif

#if MICROPY_PY_MACHINE_ADC_BLOCK
static mp_obj_t mp_machine_adc_block(machine_adc_obj_t *self);
#endif

#if MICROPY_PY_MACHINE_ADC_READ_UV
static mp_int_t mp_machine_adc_read_uv(machine_adc_obj_t *self);
#endif

#if MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH
static void mp_machine_adc_atten_set(machine_adc_obj_t *self, mp_int_t atten);
static void mp_machine_adc_width_set(machine_adc_obj_t *self, mp_int_t width);
#endif

#if MICROPY_PY_MACHINE_ADC_READ
static mp_int_t mp_machine_adc_read(machine_adc_obj_t *self);
#endif

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_ADC_INCLUDEFILE

#if MICROPY_PY_MACHINE_ADC_INIT
// ADC.init(...)
static mp_obj_t machine_adc_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_machine_adc_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_adc_init_obj, 1, machine_adc_init);
#endif

#if MICROPY_PY_MACHINE_ADC_DEINIT
// ADC.deinit()
static mp_obj_t machine_adc_deinit(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_adc_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_deinit_obj, machine_adc_deinit);
#endif

#if MICROPY_PY_MACHINE_ADC_BLOCK
// ADC.block()
static mp_obj_t machine_adc_block(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_machine_adc_block(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_block_obj, machine_adc_block);
#endif

// ADC.read_u16()
static mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_adc_read_u16(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

#if MICROPY_PY_MACHINE_ADC_READ_UV
// ADC.read_uv()
static mp_obj_t machine_adc_read_uv(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_adc_read_uv(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_uv_obj, machine_adc_read_uv);
#endif

#if MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH

// ADC.atten(value) -- this is a legacy method.
static mp_obj_t machine_adc_atten(mp_obj_t self_in, mp_obj_t atten_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t atten = mp_obj_get_int(atten_in);
    mp_machine_adc_atten_set(self, atten);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_adc_atten_obj, machine_adc_atten);

// ADC.width(value) -- this is a legacy method.
static mp_obj_t machine_adc_width(mp_obj_t self_in, mp_obj_t width_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t width = mp_obj_get_int(width_in);
    mp_machine_adc_width_set(self, width);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_adc_width_obj, machine_adc_width);

#endif

#if MICROPY_PY_MACHINE_ADC_READ
// ADC.read() -- this is a legacy method.
static mp_obj_t machine_adc_read(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_adc_read(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_obj, machine_adc_read);
#endif

static const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    #if MICROPY_PY_MACHINE_ADC_INIT
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_adc_init_obj) },
    #endif
    #if MICROPY_PY_MACHINE_ADC_DEINIT
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_adc_deinit_obj) },
    #endif
    #if MICROPY_PY_MACHINE_ADC_BLOCK
    { MP_ROM_QSTR(MP_QSTR_block), MP_ROM_PTR(&machine_adc_block_obj) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
    #if MICROPY_PY_MACHINE_ADC_READ_UV
    { MP_ROM_QSTR(MP_QSTR_read_uv), MP_ROM_PTR(&machine_adc_read_uv_obj) },
    #endif

    // Legacy methods.
    #if MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH
    { MP_ROM_QSTR(MP_QSTR_atten), MP_ROM_PTR(&machine_adc_atten_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&machine_adc_width_obj) },
    #endif
    #if MICROPY_PY_MACHINE_ADC_READ
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_adc_read_obj) },
    #endif

    // A port must add ADC class constants defining the following macro.
    // It can be defined to nothing if there are no constants.
    MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS
};
static MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_adc_make_new,
    print, mp_machine_adc_print,
    locals_dict, &machine_adc_locals_dict
    );

#endif // MICROPY_PY_MACHINE_ADC
