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
#include "extmod/modujson.h"
#include "py/objstringio.h"
#include "py/modbuiltins.h"
#include "py/objint.h"
#include "py/bc0.h"

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

void caught_exception(mp_obj_exception_t* exception, tvm_execute_result_t *result) {
    mp_obj_exception_t* o = exception;
    const char * exception_name = qstr_str(o->base.type->name);
    int error_code = 1001;
    if (strcmp(exception_name, "GasNotEnoughException") == 0) {
        error_code = 1002;
    }else if (strcmp(exception_name, "ABICheckException") == 0) {
        error_code = 2002;
    }

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);
//    mp_obj_print_exception(&print, o);
    mp_obj_get_exception_str(&print, o);
    mp_obj_fill_exception(vstr.buf, error_code, result);
    vstr_clear(&vstr);
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
        caught_exception(MP_OBJ_FROM_PTR(nlr.ret_val), result);
    }
}

// 0.03814697265625	per memory byte
// 500000gas / 0.04 = 11.9mb
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

void tvm_execute(const char *script, const char *alias, tvm_parse_kind_t parseKind, tvm_execute_result_t *result) {
	execute_from_str(script, alias, parseKind, result);
}

//storage
typedef struct _mp_obj_storage_del_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
} mp_obj_storage_del_fun_t;


STATIC mp_obj_t storage_del_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_t key = args[1];
    if (mp_obj_is_qstr(key)) {
        	byte code = MP_BC_REMOVE_STATE;
            if (!CheckGas(&code)) {
                return mp_const_none;
            }
            size_t key_len = 0;
            const char* key_c = mp_obj_str_get_data(key, &key_len);
            storage_remove_data_fn(key_c, key_len);
    } else {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_TypeError, key));
    }
    return mp_const_none;
}

STATIC const mp_obj_type_t mp_type_fun_storage_del = {
        { &mp_type_type },
        .name = MP_QSTR_function,
        .call = storage_del_call,
};

typedef struct _mp_obj_storage_set_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
} mp_obj_storage_set_fun_t;


STATIC mp_obj_t storage_set_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_t key = args[1];
    mp_obj_t value = args[2];
    if (mp_obj_is_qstr(key)) {
        byte code = MP_BC_SET_STATE;
        if(!CheckGas(&code)) {
            return mp_const_none;
        } else {
            size_t key_len = 0;
            const char *key_c = mp_obj_str_get_data(key, &key_len);
            byte *storage_value = NULL;
            size_t storage_value_len = 0;
            mp_obj_storage_value(value, &storage_value, &storage_value_len);
            FireGas_DB(key_len + storage_value_len);
            storage_set_data_fn(key_c, key_len, (char*)storage_value, storage_value_len);
            free(storage_value);
        }
    } else {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_TypeError, key));
    }
    return mp_const_none;
}

STATIC const mp_obj_type_t mp_type_fun_storage_set = {
        { &mp_type_type },
        .name = MP_QSTR_function,
        .call = storage_set_call,
};

typedef struct _mp_obj_storage_get_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
} mp_obj_storage_get_fun_t;


STATIC mp_obj_t storage_get_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_t r = NULL;
    mp_obj_t key = args[1];
    size_t len = 0;
    const char *key_c = mp_obj_str_get_data(key, &len);
    r = mp_obj_new_storage_value(self_in, key_c, len);
    return r;
}

STATIC const mp_obj_type_t mp_type_fun_storage_get = {
        { &mp_type_type },
        .name = MP_QSTR_function,
        .call = storage_get_call,
};

//__init__ hook
typedef struct _mp_obj_init_hook_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
} mp_obj_init_hook_fun_t;


STATIC mp_obj_t init_hook_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    return mp_const_none;
}

STATIC const mp_obj_type_t mp_type_fun_init_hook = {
        { &mp_type_type },
        .name = MP_QSTR_function,
        .call = init_hook_call,
};

void tvm_fun_call(const char *class_name, const char *func_name, const char *json_args, tvm_execute_result_t *result) {
    nlr_buf_t nlr;
    mp_obj_t data = NULL;
    if (nlr_push(&nlr) == 0) {
        unsigned int params_data = 0;
        mp_obj_register_t *reg = mp_load_name(qstr_from_str("register"));
        size_t public_funcs_num;
        mp_obj_t *public_funcs;
        mp_obj_list_get(reg->public_funcs, &public_funcs_num, &public_funcs);
        bool is_deploy = (strcmp(func_name, "__init__") == 0);
        size_t n_args;
        mp_obj_t *items;
        if (!is_deploy) {
            // find function info
            for(int i=0; i<public_funcs_num; i++) {
                mp_obj_decorator_fun_t* func = MP_OBJ_TO_PTR(public_funcs[i]);
                if (func->func != NULL && strcmp(func->func, func_name) == 0) {
                    params_data = func->params_data;
                    break;
                } else if (i == public_funcs_num-1) {
                    char error[80];
                    sprintf(error, "%s is not a public function", func_name);
                    mp_raise_ABICheckException(error);
                }
            }

            if (json_args != NULL) {
                size_t len = strlen(json_args);
                vstr_t vstr = {len, len, (char*)json_args, true};
                mp_obj_stringio_t sio = {{&mp_type_stringio}, &vstr, 0, MP_OBJ_NULL};
                mp_obj_t args = mod_ujson_load(MP_OBJ_FROM_PTR(&sio));
                if (!mp_obj_is_type(args, &mp_type_list)) {
                    mp_raise_ABICheckException("not a params list");
                }
                mp_obj_list_get(args, &n_args, &items);
            } else {
                n_args = 0;
            }

            // check params num
            if (get_type_num(params_data) != n_args) {
                char error[80];
                sprintf(error, "function: %s's params num error", func_name);
                mp_raise_ABICheckException(error);
            }
            // check params type
            for (int i=0; i<(int)n_args; i++) {
                const char* wanted_type = get_type_msg(params_data, i);
                if (strcmp(wanted_type, mp_obj_get_type_str(items[i])) != 0) {
                    char error[80];
                    sprintf(error, "function: %s's params type error", func_name);
                    mp_raise_ABICheckException(error);
                }
            }
        }

        mp_obj_t class = mp_load_name(qstr_from_str(class_name));

        mp_obj_storage_set_fun_t *storage_set = m_new_obj(mp_obj_storage_set_fun_t);
        storage_set->base.type = &mp_type_fun_storage_set;
        mp_store_attr(class, qstr_from_str("__setattr__"), storage_set);

        mp_obj_storage_get_fun_t *storage_get = m_new_obj(mp_obj_storage_get_fun_t);
        storage_get->base.type = &mp_type_fun_storage_get;
        mp_store_attr(class, qstr_from_str("__getattr__"), storage_get);

        mp_obj_storage_del_fun_t *storage_del = m_new_obj(mp_obj_storage_del_fun_t);
        storage_del->base.type = &mp_type_fun_storage_del;
        mp_store_attr(class, qstr_from_str("__delattr__"), storage_del);

        if (is_deploy) {
            data = mp_call_function_0(class);
        } else {
            mp_obj_init_hook_fun_t *init_hook = m_new_obj(mp_obj_init_hook_fun_t);
            init_hook->base.type = &mp_type_fun_init_hook;
            mp_store_attr(class, qstr_from_str("__init__"), init_hook);

            mp_obj_t class_object = mp_call_function_0(class);
            if (n_args == 0) {
                data = mp_call_function_0(mp_load_attr(class_object, qstr_from_str(func_name)));
            } else {
                data = mp_call_function_n_kw(mp_load_attr(class_object, qstr_from_str(func_name)), n_args, 0, items);
            }
        }
        nlr_pop();
        mp_fun_return(data, result);
    } else {
        caught_exception(MP_OBJ_FROM_PTR(nlr.ret_val), result);
    }
}

void tvm_set_register() {
    mp_obj_t msg_class = mp_load_name(qstr_from_str("Register"));

    mp_obj_t msg_object = mp_call_function_0(msg_class);

    mp_store_global(qstr_from_str("register"), msg_object);
}


void tvm_set_msg(const char* sender, unsigned long long value) {
    mp_obj_t msg_class = mp_load_name(qstr_from_str("Msg"));

    mp_obj_t msg_object = mp_call_function_2(msg_class, mp_obj_new_str(sender, strlen(sender)), mp_obj_new_int_from_ull(value));

    mp_store_global(qstr_from_str("msg"), msg_object);
}

/***********************/

void tvm_set_gas(int limit) {
	setGas(limit);
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
const int MAX_PARAMS_NUM = 8;

const char*supported_type[] = {"", "int","str","bool", "list","dict"};

int is_supported_type(const char* t) {
    for(int i=1; i < sizeof(supported_type)/ sizeof(supported_type[0]); i++)
        if (strcmp(t, supported_type[i]) == 0) {
            return i;
        }
    return 0;
}

void set_type_msg(unsigned int *msg, int num, int type_index) {
    if (num >= MAX_PARAMS_NUM) {
        return;
    }
    *msg = *msg |(type_index << num * 4);
}

const char* get_type_msg(unsigned int msg, int num) {
    if (num >= MAX_PARAMS_NUM) {
        return supported_type[0];
    }
    int type_index = 0x0f & (msg >> num * 4);
    if (type_index >= sizeof(supported_type)/ sizeof(supported_type[0])) {
        type_index = 0;
    }
    return supported_type[type_index];
}

int get_type_num(unsigned int msg) {
    for(int i=0; i < MAX_PARAMS_NUM; i++) {
        if ((0x0f & (msg >> i * 4)) == 0) {
            return i;
        }
    }
    return MAX_PARAMS_NUM;
}

const char* tvm_export_abi() {
    unsigned int params_data = 0;
    mp_obj_register_t *reg = mp_load_name(qstr_from_str("register"));
    size_t public_funcs_num;
    mp_obj_t *public_funcs;
    mp_obj_list_get(reg->public_funcs, &public_funcs_num, &public_funcs);
    mp_obj_list_t *abi = mp_obj_new_list(0, NULL);
    for(int i=0; i<public_funcs_num; i++) {
        mp_obj_decorator_fun_t* func = MP_OBJ_TO_PTR(public_funcs[i]);
        if (func->func != NULL) {
            params_data = func->params_data;
            mp_obj_dict_t* items = mp_obj_new_dict(2);
            mp_obj_dict_store(items, mp_obj_new_str("func_name", 9), mp_obj_new_str(func->func, strlen(func->func)));
            mp_obj_list_t *params = mp_obj_new_list(0, NULL);
            int params_num = get_type_num(params_data);
            for(int j=0;j<params_num;j++) {
                const char* t = get_type_msg(params_data, j);
                mp_obj_list_append(params, mp_obj_new_str(t, strlen(t)));
            }
            mp_obj_dict_store(items, mp_obj_new_str("args", 4),params);
            mp_obj_list_append(abi, items);
        }
    }
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);
    mp_obj_print_helper(&print, abi, PRINT_JSON);
    const char *params_json = vstr_str(&vstr);
    return params_json;
}

