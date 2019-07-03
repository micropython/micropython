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

#ifndef TVM_TVM_H
#define TVM_TVM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    //TODO none
    RETURN_TYPE_INT = 1,
    RETURN_TYPE_STRING,
    RETURN_TYPE_NONE,
    RETURN_TYPE_EXCEPTION,
    RETURN_TYPE_BOOL,
}tvm_return_type_t;

//parse.h mp_parse_input_kind_t
typedef enum {
    PARSE_KIND_SINGLE = 0,
    PARSE_KIND_FILE,
    PARSE_KIND_EVAL,
}tvm_parse_kind_t;

typedef struct _tvm_execute_result_t {
    int result_type; //tvm_return_type_t
    int error_code;
    char *content;
    char *abi;
}tvm_execute_result_t;
void tvm_print_result(tvm_execute_result_t *result);
void tvm_init_result(tvm_execute_result_t *result);
void tvm_deinit_result(tvm_execute_result_t *result);

void tvm_execute(const char *script, const char *alias, tvm_parse_kind_t parseKind, tvm_execute_result_t *result);

void tvm_set_lib_line(int line);

void tvm_set_lib_path(const char* path);

void tvm_start();
void tvm_gc();
void tvm_delete();

void tvm_create_context();
void tvm_remove_context();

void tvm_set_gas(int limit);
int tvm_get_gas();
void tvm_gas_report();

// account
typedef char* (*get_balance_fn_t) (const char*);
get_balance_fn_t get_balance;
typedef _Bool (*miner_stake_fn_t)(const char*, int len, const char*);
miner_stake_fn_t miner_stake_fn;
typedef _Bool (*miner_cancel_stake_fn_t)(const char*, int len, const char*);
miner_cancel_stake_fn_t miner_cancel_stake;
typedef _Bool (*miner_refund_stake_fn_t)(const char*, int len);
miner_refund_stake_fn_t miner_refund_stake;

// storage
typedef char* (*storage_get_data_fn_t) (const char*);
storage_get_data_fn_t storage_get_data_fn;
typedef void (*storage_set_data_fn_t) (const char*, const char*);
storage_set_data_fn_t storage_set_data_fn;
typedef void (*storage_remove_data_fn_t) (const char*);
storage_remove_data_fn_t storage_remove_data_fn;

// block
typedef char* (*block_hash_fn_t) (unsigned long long);
block_hash_fn_t block_hash_fn;
typedef unsigned long long (*block_number_fn_t) ();
block_number_fn_t block_number_fn;
typedef unsigned long long (*block_timestamp_fn_t) ();
block_timestamp_fn_t block_timestamp_fn;

// other
typedef void (*contract_call_fn_t) (const char*, const char*, const char*, tvm_execute_result_t *result);
contract_call_fn_t contract_call_fn;
typedef char* (*event_call_fn_t) (const char*, const char*);
event_call_fn_t event_call_fn;
typedef _Bool (*transfer_fn_t) (const char*, const char*);
transfer_fn_t transfer_fn;
typedef unsigned long long (*gas_limit_fn_t) ();
gas_limit_fn_t gas_limit_fn;

#ifdef __cplusplus
}
#endif
#endif //TVM_TVM_H
