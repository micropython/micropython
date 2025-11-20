/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

#include "py/runtime.h"
#include "py/mphal.h"

// PIC32MX Timer base addresses
#define TMR1_BASE  0xBF800600
#define TMR2_BASE  0xBF800800
#define TMR3_BASE  0xBF800A00
#define TMR4_BASE  0xBF800C00
#define TMR5_BASE  0xBF800E00

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    uint8_t timer_id;
    uint32_t period;
    mp_obj_t callback;
} machine_timer_obj_t;

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    machine_timer_obj_t *self = mp_obj_malloc(machine_timer_obj_t, type);
    self->timer_id = mp_obj_get_int(args[0]);
    self->period = 1000;  // 1ms default
    self->callback = mp_const_none;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // TODO: Parse mode, period, callback from kwargs
    // TODO: Configure timer peripheral

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // TODO: Disable timer
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },

    // Timer modes
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(1) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    locals_dict, &machine_timer_locals_dict
);
