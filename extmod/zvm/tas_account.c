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
#include "tas_account.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/bc0.h"
#include "py/objint.h"
#include "py/lexer.h"
#include "py/compile.h"
#ifndef _MYWINDOWS_
#include "py/tvm.h"
#endif
#include "py/gas.h"
#include "py/obj.h"
#include <stdlib.h>
#include <string.h>


int inputValCheck(mp_obj_t val){
    if (MP_OBJ_IS_SMALL_INT(val)) {
        return 1;
    } else {
        const mp_obj_int_t *self = MP_OBJ_TO_PTR(val);
        mp_int_t value;
        if (mpz_as_int_checked(&self->mpz, &value)) {
            return 2;
        } else {
            // overflow
            printf("number overflow, nothing to change, please confirm your input!\n");
            return 0;
        }
    }
}

STATIC mp_obj_t mod_account_contract_call(mp_obj_t contractAddr, mp_obj_t contractName, mp_obj_t contractArgs) {
    const char *addr = mp_obj_str_get_str(contractAddr);
    const char *name = mp_obj_str_get_str(contractName);
    const char *args = mp_obj_str_get_str(contractArgs);
    tvm_execute_result_t result;
    tvm_init_result(&result);
    contract_call_fn(addr, name, args, &result);
	mp_obj_t return_obj =  result_to_obj(&result);
	tvm_deinit_result(&result);
	return return_obj;
};

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_account_contract_call_obj , mod_account_contract_call);

//extern int g_iGas;
STATIC mp_obj_t mod_account_event_call(mp_obj_t EventName, mp_obj_t Index, mp_obj_t Data) {
	byte code = MP_BC_TASEVENT;
	if (!CheckGas(&code)) {
		return mp_const_none;
	}

	const char *name = mp_obj_str_get_str(EventName);
	const char *index = mp_obj_str_get_str(Index);
	const char *data = mp_obj_str_get_str(Data);
	event_call_fn(name, index, data);
	return MP_OBJ_FROM_PTR(&mp_const_none_obj);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_account_event_call_obj, mod_account_event_call);

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
    FireGas_DB(key_len + value_len);
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
        { MP_ROM_QSTR(MP_QSTR_contract_call), MP_ROM_PTR(&mod_account_contract_call_obj) },
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




