//   Copyright (C) 2018 TASChain
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include "tas.h"

#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/lexer.h"
#include "py/compile.h"
#include "py/tvm.h"
#include <stdlib.h>
#include <string.h>

typedef struct _mp_obj_tasaccount_t {
    mp_obj_base_t base;
    mp_obj_t teststr;
} mp_obj_tasaccount_t;


STATIC void tasaccount_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_tasaccount_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "my tasaccount %p", self);
}

STATIC mp_obj_t tasaccount_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_obj_tasaccount_t *o = m_new_obj(mp_obj_tasaccount_t);
    o->base.type = type;
    const char str[] = "teststr";
    o->teststr = mp_obj_new_str(str, sizeof(str) - 1);
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t tasaccount_hello(size_t n_args, const mp_obj_t *args) {
    return MP_OBJ_NULL;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tasaccount_hello_obj, 0, tasaccount_hello);

STATIC const mp_rom_map_elem_t tasaccount_locals_dict_table[] = {
{
    MP_ROM_QSTR(MP_QSTR_hello), MP_ROM_PTR(&tasaccount_hello_obj) },
};
STATIC MP_DEFINE_CONST_DICT(tasaccount_locals_dict, tasaccount_locals_dict_table);

STATIC void tasaccount_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_tasaccount_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        dest[0] = self->teststr;
    } else {
        // delete/store attribute
        self->teststr = dest[1];
        dest[0] = MP_OBJ_NULL; // indicate success
    }
}

STATIC const mp_obj_type_t tasaccount_type = {
        { &mp_type_type },
        .name = MP_QSTR_tasaccount,
        .print = tasaccount_print,
        .make_new = tasaccount_make_new,
        .locals_dict = (void*)&tasaccount_locals_dict,
        .attr = tasaccount_attr,
};


////////////


STATIC const mp_rom_map_elem_t mp_module_tas_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tas) },
    { MP_ROM_QSTR(MP_QSTR_tasaccount), MP_ROM_PTR(&tasaccount_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_tas_globals, mp_module_tas_globals_table);

const mp_obj_module_t mp_module_tas = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_tas_globals,
};


