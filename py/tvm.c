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

#include "tvm.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "extmod/misc.h"
#include "py/builtin.h"
#include "py/gas.h"
#include "py/objfun.h"


uint mp_import_stat(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else if (S_ISREG(st.st_mode)) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}

//static char heap[16384];
long heap_size = 1024 * 1024 * (sizeof(mp_uint_t) / 4);
void execute_from_str(const char *str, const char *file_name, uint emit_opt, tvm_execute_result_t *result) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = NULL;
        if (file_name != NULL)
			lex = mp_lexer_new_from_str_len(qstr_from_str(file_name), str, strlen(str), false);
        else
			lex = mp_lexer_new_from_str_len(0/*MP_QSTR_*/, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, emit_opt);
        mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, false);
		mp_obj_t data = mp_call_function_0(module_fun);
        nlr_pop();
        mp_fun_return(data, result);
    } else {
		mp_obj_exception_t *o = MP_OBJ_TO_PTR(MP_OBJ_FROM_PTR(nlr.ret_val));
		const char * exception_name = qstr_str(o->base.type->name);
		int error_code = 1001;
		if (strcmp(exception_name, "GasNotEnoughException") == 0) {
			error_code = 1002;
		}else if (strcmp(exception_name, "ABICheckException") == 0) {
			error_code = 2002;
		}
		
		char * exception_data_str = mp_obj_get_exception_str(MP_OBJ_FROM_PTR(nlr.ret_val), exception_name);
		assert(exception_data_str);
		mp_obj_fill_exception(exception_data_str, error_code, result);
    }
}

static char heap[1024 * 1024 * 16];
void tvm_start(void) {
	// Initialized stack limit
//	mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));
	// Initialize heap
	gc_init(heap, heap + sizeof(heap));
	// Initialize interpreter
	mp_init();
}

void tvm_gc() {
	gc_collect_start();
	gc_collect_end();
}

void tvm_delete(void) {
	mp_deinit();
}

void tvm_set_lib_path(const char *path) {
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(QSTR_FROM_STR_STATIC(path)));
}

void tvm_print_result(tvm_execute_result_t *result) {
    printf("Rsult:\n");
    printf("result_type: %d\n", result->result_type);
    printf("error_code: %d\n", result->error_code);
    if (result->content)
        printf("content: %s\n", result->content);
    if (result->abi)
        printf("abi: %s\n", result->abi);
}

void tvm_init_result(tvm_execute_result_t *result) {
    result->result_type = -1;
    result->error_code = -1;
    result->content = NULL;
    result->abi = NULL;
}

void tvm_deinit_result(tvm_execute_result_t *result) {
    result->result_type = -1;
    result->error_code = -1;
    if (result->content) {
        free(result->content);
        result->content = NULL;
    }
    if (result->abi) {
        free(result->abi);
        result->abi = NULL;
    }
}

extern char* g_pAbi;
void tvm_execute(const char *script, const char *alias, tvm_parse_kind_t parseKind, tvm_execute_result_t *result) {
	if (g_pAbi)
	{
		free(g_pAbi);
		g_pAbi = NULL;
	}
	execute_from_str(script, alias, parseKind, result);

	if (g_pAbi != NULL)
	{
		result->abi = malloc(strlen(g_pAbi) + 1);
		memset(result->abi, 0, strlen(g_pAbi) + 1);
		memcpy(result->abi, g_pAbi, strlen(g_pAbi));

		if (g_pAbi)
		{
			free(g_pAbi);
			g_pAbi = NULL;
		}
	}
}

/***********************/

void tvm_set_gas(int limit) {
	setGas(limit);
}

void tvm_set_lib_line(int line) {
	set_code_line(line);
}

int tvm_get_gas() {
	return getGas();
}
void tvm_gas_report() {
	Gas_Report();
}

/***********************/

mp_obj_dict_t* context_ary[10];
int context_ary_index = 0;

void tvm_create_context() {

	if (context_ary_index == 0) {
		context_ary[0] = mp_globals_get();
		context_ary_index++;
	}

	mp_obj_dict_t *dict_main = malloc(sizeof(mp_obj_dict_t));
	context_ary[context_ary_index++] = dict_main;
	// initialise the __main__ module
	mp_obj_dict_init(dict_main, 1);
	mp_obj_dict_store(MP_OBJ_FROM_PTR(dict_main), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR___main__));

	// locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
	mp_locals_set(dict_main);
	mp_globals_set(dict_main);
}

void tvm_remove_context() {
	mp_obj_dict_t *dict = context_ary[--context_ary_index];
	mp_map_deinit(&dict->map);
	free(dict);

	mp_obj_dict_t *dict_main = context_ary[context_ary_index-1];

	mp_globals_set(dict_main);
	mp_locals_set(dict_main);
}

/***********************/


