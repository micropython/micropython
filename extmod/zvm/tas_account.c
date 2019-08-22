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
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/bc0.h"
#include "py/objint.h"
#include "py/lexer.h"
#include "py/compile.h"
#include "py/tvm.h"
#include "py/gas.h"
#include "py/obj.h"
#include <stdlib.h>
#include <string.h>



STATIC mp_obj_t mod_account_get_balance(mp_obj_t address) {
    byte code = MP_BC_GET_BALANCE;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    const char* str = get_balance(mp_obj_str_get_str(address));
    bool neg = false;
    if (str[0] == 45) {
        neg = true;
        str ++;
    }
    int len = strlen(str);
    return mp_obj_new_int_from_str_len(&str, len, neg, 10);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_account_get_balance_obj , mod_account_get_balance);


STATIC mp_obj_t mod_account_transfer(mp_obj_t address,mp_obj_t amount) {
    bool success=false;
    mp_obj_get_int(amount);
    char stack_buf[sizeof(mp_int_t) * 4];
    char *buf = stack_buf;
    size_t buf_size = sizeof(stack_buf);
    size_t fmt_size;

    char *str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, amount, 10, NULL, '\0', '\0');
    if (buf != stack_buf) {
        m_del(char, buf, buf_size);
    }

    byte code = MP_BC_TRANSFER;
    if(CheckGas(&code)) {
        success = transfer_fn(mp_obj_str_get_str(address), str);
    }
    return mp_obj_new_bool(success);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_account_transfer_obj, mod_account_transfer);

STATIC const mp_rom_map_elem_t mp_module_account_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_account) },
        { MP_ROM_QSTR(MP_QSTR_get_balance), MP_ROM_PTR(&mod_account_get_balance_obj) },
        { MP_ROM_QSTR(MP_QSTR_transfer), MP_ROM_PTR(&mod_account_transfer_obj)},

};

STATIC MP_DEFINE_CONST_DICT(mp_module_account_globals, mp_module_account_globals_table);

const mp_obj_module_t mp_module_account = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&mp_module_account_globals,
};




