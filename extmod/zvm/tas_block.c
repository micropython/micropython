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
#include "tas_block.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/objint.h"
#include "py/lexer.h"
#include "py/compile.h"
#ifndef _MYWINDOWS_
#include "py/tvm.h"
#endif
#include "py/bc0.h"
#include "tas_account.h"
#include <stdlib.h>
#include <string.h>
#include "tas_account.h"
#include "py/gas.h"


STATIC mp_obj_t mod_block_blockhash(mp_obj_t height) {
    byte code = MP_BC_BLOCKHASH;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    if(!inputValCheck(height)) {
        return mp_const_none;
    };
    const char* str = (char*)block_hash_fn(mp_obj_get_int(height));
    int len = strlen(str);
    return mp_obj_new_str(str, len);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_block_blockhash_obj , mod_block_blockhash);

STATIC mp_obj_t mod_block_number(void) {
    byte code = MP_BC_NUMBER;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    long long unsigned int val = block_number_fn();
    return mp_obj_new_int_from_ull(val);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_block_number_obj , mod_block_number);

STATIC mp_obj_t mod_block_timestamp(void) {
    byte code = MP_BC_TIMESTAMP;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    long long unsigned int val = block_timestamp_fn();
    return mp_obj_new_int_from_ull(val);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_block_timestamp_obj , mod_block_timestamp);

STATIC mp_obj_t mod_block_gaslimit(void) {
    byte code = MP_BC_GASLIMIT;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    long long unsigned int val = gas_limit_fn();
    return mp_obj_new_int_from_ull(val);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_block_gaslimit_obj , mod_block_gaslimit);


STATIC const mp_rom_map_elem_t mp_module_block_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_block) },
    { MP_ROM_QSTR(MP_QSTR_blockhash), MP_ROM_PTR(&mod_block_blockhash_obj) },
        { MP_ROM_QSTR(MP_QSTR_number), MP_ROM_PTR(&mod_block_number_obj) },
    { MP_ROM_QSTR(MP_QSTR_timestamp), MP_ROM_PTR(&mod_block_timestamp_obj) },
    { MP_ROM_QSTR(MP_QSTR_gaslimit), MP_ROM_PTR(&mod_block_gaslimit_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_block_globals, mp_module_block_globals_table);

const mp_obj_module_t mp_module_block = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&mp_module_block_globals,
};




