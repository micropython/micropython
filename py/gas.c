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

#include "py/gas.h"

#include <stdio.h>

#include "py/bc0.h"
#include "py/runtime.h"

typedef enum gasPrice
{
    Zero = 0,	// 0, Zero
    Base = 2,	// 2, Quick
    VeryLow=3,	// 3, Fastest
    Low=5,		// 5, Fast
    Mid=8,		// 8, Mid
    High=10,	// 10, Slow
    Ext=20,		// 20, Ext

    getBalance=20,

    getState=200,
    setState=200,
	removeState=200,

    transfer=400,

	tasevent=8,

    blockhash=20,
    number=20,
    timestamp=20,
    gaslimit=20,

    origin = 20,

//	Special,	// multiparam or otherwise special
//	Invalid		// Invalid.

}GASPRICE;

typedef struct stCodeGas {
    byte		bCode;
    GASPRICE	iGas;
}STCODEGAS, *PSTCODEGAS;

STCODEGAS g_CodeGas[] = {
        { MP_BC_LOAD_CONST_FALSE ,Base },
        { MP_BC_LOAD_CONST_NONE ,Base },
        { MP_BC_LOAD_CONST_TRUE ,Base },
        { MP_BC_LOAD_CONST_SMALL_INT,Base },
        { MP_BC_LOAD_CONST_STRING ,VeryLow },
        { MP_BC_LOAD_CONST_OBJ ,VeryLow },
        { MP_BC_LOAD_NULL ,Base },
        { MP_BC_LOAD_FAST_N ,Low },
        { MP_BC_LOAD_DEREF ,Base },
        { MP_BC_LOAD_NAME ,Low },
        { MP_BC_LOAD_GLOBAL ,Low },
        { MP_BC_LOAD_ATTR ,VeryLow },
        { MP_BC_LOAD_METHOD ,Low },
        { MP_BC_LOAD_SUPER_METHOD ,Low },
        { MP_BC_LOAD_BUILD_CLASS ,VeryLow },
        { MP_BC_LOAD_SUBSCR ,Mid },
        { MP_BC_STORE_FAST_N ,Low },
        { MP_BC_STORE_DEREF ,Low },
        { MP_BC_STORE_NAME ,Low },
        { MP_BC_STORE_GLOBAL ,Low },
        { MP_BC_STORE_ATTR ,Low },
        { MP_BC_STORE_SUBSCR ,Low },
        { MP_BC_DELETE_FAST ,Mid },
        { MP_BC_DELETE_DEREF ,Mid },
        { MP_BC_DELETE_NAME ,Mid },
        { MP_BC_DELETE_GLOBAL ,Mid },
        { MP_BC_DUP_TOP ,Base },
        { MP_BC_DUP_TOP_TWO ,Base },
        { MP_BC_POP_TOP ,Base },
        { MP_BC_ROT_TWO ,Base },
        { MP_BC_ROT_THREE ,Base },
        { MP_BC_JUMP ,Base },
        { MP_BC_POP_JUMP_IF_TRUE ,Base },
        { MP_BC_POP_JUMP_IF_FALSE ,Base },
        { MP_BC_JUMP_IF_TRUE_OR_POP ,Base },
        { MP_BC_JUMP_IF_FALSE_OR_POP ,Base },
        { MP_BC_SETUP_WITH ,Mid },
        { MP_BC_WITH_CLEANUP ,High },
        { MP_BC_SETUP_EXCEPT ,Base },
        { MP_BC_SETUP_FINALLY ,Base },
        { MP_BC_END_FINALLY ,Zero },
        { MP_BC_GET_ITER ,VeryLow },
        { MP_BC_FOR_ITER ,Base },
//        { MP_BC_POP_BLOCK ,Base },
//        { MP_BC_POP_EXCEPT ,Base },
        { MP_BC_POP_EXCEPT_JUMP, Base},
        { MP_BC_UNWIND_JUMP ,Mid },
        { MP_BC_GET_ITER_STACK ,Base },
        { MP_BC_BUILD_TUPLE ,VeryLow },
        { MP_BC_BUILD_LIST ,VeryLow },
        { MP_BC_BUILD_MAP ,VeryLow },
        { MP_BC_STORE_MAP ,VeryLow },
        { MP_BC_BUILD_SET ,VeryLow },
        { MP_BC_BUILD_SLICE ,VeryLow },
        { MP_BC_STORE_COMP ,Base },
        { MP_BC_UNPACK_SEQUENCE ,VeryLow },
        { MP_BC_UNPACK_EX ,VeryLow },
        { MP_BC_RETURN_VALUE ,Base },
        { MP_BC_RAISE_VARARGS ,Low },
        { MP_BC_YIELD_VALUE ,VeryLow },
        { MP_BC_YIELD_FROM ,Ext },
        { MP_BC_MAKE_FUNCTION ,Ext },
        { MP_BC_MAKE_FUNCTION_DEFARGS ,Ext },
        { MP_BC_MAKE_CLOSURE ,Ext },
        { MP_BC_MAKE_CLOSURE_DEFARGS ,Ext },
        { MP_BC_CALL_FUNCTION ,Ext },
        { MP_BC_CALL_FUNCTION_VAR_KW ,Ext },
        { MP_BC_CALL_METHOD ,Ext },
        { MP_BC_CALL_METHOD_VAR_KW ,Ext },
        { MP_BC_IMPORT_NAME ,Ext },
        { MP_BC_IMPORT_FROM ,Ext },
        { MP_BC_IMPORT_STAR ,Base },

        {MP_BC_GET_BALANCE, getBalance},
        {MP_BC_GET_STATE, getState},
        {MP_BC_SET_STATE, setState},
		{MP_BC_REMOVE_STATE, removeState},

        {MP_BC_TRANSFER, transfer},

		{ MP_BC_TASEVENT, tasevent},

//BLOCK
        {MP_BC_BLOCKHASH, blockhash},
        {MP_BC_NUMBER, number},
        {MP_BC_TIMESTAMP,timestamp},
        {MP_BC_GASLIMIT, gaslimit},

//TX
        {MP_BC_ORIGIN, origin},

};

long long g_iGas = 100000;

#define GAS_PRECISION 10000

int db_gas = 0;
int mem_gas = 0;
int cpu_gas = 0;

void setGas(int value) {
    g_iGas = (long long)value * GAS_PRECISION;
    db_gas = 0;
    mem_gas = 0;
    cpu_gas = 0;
}

int getGas() {
    return g_iGas / GAS_PRECISION;
}

GASPRICE GetGas(byte* op)
{
    int i = 0;
    for (i = 0; i < sizeof(g_CodeGas) / sizeof(STCODEGAS); i++)
    {
        if (g_CodeGas[i].bCode == *op)
        {
            return g_CodeGas[i].iGas;
        }
    }

    if (op[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + 64) {
        return Base;
    }
    else if (op[-1] < MP_BC_LOAD_FAST_MULTI + 16) {
        return Base;
    }
    else if (op[-1] < MP_BC_STORE_FAST_MULTI + 16) {
        return Base;
    }
    else if (op[-1] < MP_BC_UNARY_OP_MULTI + MP_UNARY_OP_NUM_BYTECODE) {
        return Base;
    }
    else if (op[-1] < MP_BC_BINARY_OP_MULTI + MP_BINARY_OP_NUM_BYTECODE) {
        return Base;
    }
    else {
        return Base;
    }

    return 0;//not find ,so it should be big value
}

bool FireGas(int gas)
{
    if (g_iGas - gas >= 0) {
        g_iGas -= gas;
        return true;
    }
    else {
        g_iGas = 0;
        return false;
    }
}

bool CheckGas(byte* op)
{
    GASPRICE gas = GetGas(op);
    int gas_used = gas * GAS_PRECISION;
    cpu_gas += gas_used;
    if (FireGas(gas_used)) {
        return true;
    }
    else {
        mp_raise_GasNotEnoughError("dose not have enough gas to run!");
        return false;
    }
}

bool FireGas_Mem(size_t len)
{
    const int MAX = INT_MAX / 381;
    if (len < MAX) {
        len = len * 381;//(int)(0.03814697265625 * GAS_PRECISION);
        mem_gas += len;
        return FireGas(len);
    } else {
        g_iGas = 0;
        return false;
    }
}

bool FireGas_DB(size_t len)
{
    const int MAX = INT_MAX / 3814;
    if (len < MAX) {
        len = len * 3814;//(int)(0.3814697265625 * GAS_PRECISION);
        db_gas += len;
        return FireGas(len);
    } else {
        g_iGas = 0;
        return false;
    }
}

void Gas_Report()
{
    printf("gas: cpu:%d mem:%d db:%d\n", cpu_gas/GAS_PRECISION, mem_gas/GAS_PRECISION, db_gas/GAS_PRECISION);
}







