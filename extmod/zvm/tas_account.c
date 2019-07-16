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



//extern int g_iGas;
STATIC mp_obj_t mod_account_event_call(mp_obj_t EventName, mp_obj_t Data) {
	byte code = MP_BC_TASEVENT;
	if (!CheckGas(&code)) {
		return mp_const_none;
	}

	const char *name = mp_obj_str_get_str(EventName);
	const char *data = mp_obj_str_get_str(Data);
    size_t name_len = strlen(name);
    size_t data_len = strlen(data);
    if (!FireGas_DB(name_len + data_len)) {
        return mp_const_none;
    }
	event_call_fn(name, data);
	return MP_OBJ_FROM_PTR(&mp_const_none_obj);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_account_event_call_obj, mod_account_event_call);

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

STATIC mp_obj_t mod_account_get_data(mp_obj_t address) {
    byte code = MP_BC_GET_STATE;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    const char* str = storage_get_data_fn(mp_obj_str_get_str(address));
    int len = strlen(str);
    return mp_obj_new_str(str, len);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_account_get_data_obj , mod_account_get_data);
//
STATIC mp_obj_t mod_account_set_Data(mp_obj_t key, mp_obj_t value) {
    byte code = MP_BC_SET_STATE;
    if(!CheckGas(&code)) {
        return mp_const_none;
    }
    const char *c_key = mp_obj_str_get_str(key);
    size_t key_len = strlen(c_key);
    const char *c_value = mp_obj_str_get_str(value);
    size_t value_len = strlen(c_value);
    if (!FireGas_DB(key_len + value_len)) {
        return mp_const_none;
    }
    storage_set_data_fn(mp_obj_str_get_str(key), mp_obj_str_get_str(value));
    return mp_const_none;
};

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_account_set_Data_obj , mod_account_set_Data);
//
STATIC mp_obj_t mod_account_remove_data(mp_obj_t key) {
	byte code = MP_BC_REMOVE_STATE;
	if (!CheckGas(&code)) {
		return mp_const_none;
	}
	storage_remove_data_fn(mp_obj_str_get_str(key));
	return mp_const_none;
};

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_account_remove_data_obj, mod_account_remove_data);

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

STATIC mp_obj_t mod_account_stake(mp_obj_t address, mp_obj_t _type, mp_obj_t amount) {
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
        success = miner_stake_fn(mp_obj_str_get_str(address), mp_obj_get_int(_type), str);
    }
    return mp_obj_new_bool(success);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_account_stake_obj, mod_account_stake);

STATIC mp_obj_t mod_account_cancel_stake(mp_obj_t address, mp_obj_t _type, mp_obj_t amount) {
    bool success = false;
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
        success = miner_cancel_stake(mp_obj_str_get_str(address), mp_obj_get_int(_type), str);
    }
    return mp_obj_new_bool(success);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_account_cancel_stake_obj, mod_account_cancel_stake);

STATIC mp_obj_t mod_account_refund_stake(mp_obj_t address, mp_obj_t _type) {
    bool success=false;

    byte code = MP_BC_TRANSFER;
    if(CheckGas(&code)) {
        success = miner_refund_stake(mp_obj_str_get_str(address), mp_obj_get_int(_type));
    }
    return mp_obj_new_bool(success);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_account_refund_stake_obj, mod_account_refund_stake);

STATIC const mp_rom_map_elem_t mp_module_account_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_account) },
		{ MP_ROM_QSTR(MP_QSTR_event_call), MP_ROM_PTR(&mod_account_event_call_obj) },
        { MP_ROM_QSTR(MP_QSTR_get_balance), MP_ROM_PTR(&mod_account_get_balance_obj) },
        { MP_ROM_QSTR(MP_QSTR_get_data), MP_ROM_PTR(&mod_account_get_data_obj) },
        { MP_ROM_QSTR(MP_QSTR_set_data), MP_ROM_PTR(&mod_account_set_Data_obj) },
		{ MP_ROM_QSTR(MP_QSTR_remove_data), MP_ROM_PTR(&mod_account_remove_data_obj) },
        { MP_ROM_QSTR(MP_QSTR_transfer), MP_ROM_PTR(&mod_account_transfer_obj)},
        { MP_ROM_QSTR(MP_QSTR_stake), MP_ROM_PTR(&mod_account_stake_obj)},
        { MP_ROM_QSTR(MP_QSTR_cancel_stake), MP_ROM_PTR(&mod_account_cancel_stake_obj)},
        { MP_ROM_QSTR(MP_QSTR_refund_stake), MP_ROM_PTR(&mod_account_refund_stake_obj)},

};

STATIC MP_DEFINE_CONST_DICT(mp_module_account_globals, mp_module_account_globals_table);

const mp_obj_module_t mp_module_account = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&mp_module_account_globals,
};




