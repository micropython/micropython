// in principle, rt_xxx functions are called only by vm/native/viper and make assumptions about args
// mp_xxx functions are safer and can be called by anyone
// note that rt_assign_xxx are called only from emit*, and maybe we can rename them to reflect this

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "parsenum.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"
#include "objarray.h"
#include "bc.h"

#if MICROPY_RUNTIME_DEBUG  // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_CODE (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(args...) DEBUG_printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#define DEBUG_OP_printf(args...) (void)0
#endif

// locals and globals need to be pointers because they can be the same in outer module scope
STATIC mp_map_t *map_locals;
STATIC mp_map_t *map_globals;
STATIC mp_map_t map_builtins;
STATIC mp_map_t map_loaded_modules; // TODO: expose as sys.modules

typedef enum {
    MP_CODE_NONE,
    MP_CODE_BYTE,
    MP_CODE_NATIVE,
    MP_CODE_INLINE_ASM,
} mp_code_kind_t;

typedef struct _mp_code_t {
    struct {
        mp_code_kind_t kind : 8;
        uint scope_flags : 8;
    };
    struct {
        uint n_args : 16;
        uint n_state : 16;
    };
    union {
        struct {
            byte *code;
            uint len;
        } u_byte;
        struct {
            mp_fun_t fun;
        } u_native;
        struct {
            void *fun;
        } u_inline_asm;
    };
    qstr *arg_names;
} mp_code_t;

STATIC uint next_unique_code_id;
STATIC machine_uint_t unique_codes_alloc = 0;
STATIC mp_code_t *unique_codes = NULL;

#ifdef WRITE_CODE
FILE *fp_write_code = NULL;
#endif

// builtins
// we put this table in ROM because it's always needed and takes up quite a bit of room in RAM
// in fact, it uses less ROM here in table form than the equivalent in code form initialising a dynamic mp_map_t object in RAM
// at the moment it's a linear table, but we could convert it to a const mp_map_t table with a simple preprocessing script
// if we wanted to allow dynamic modification of the builtins, we could provide an mp_map_t object which is searched before this one

typedef struct _mp_builtin_elem_t {
    qstr qstr;
    mp_obj_t fun;
} mp_builtin_elem_t;

STATIC const mp_builtin_elem_t builtin_table[] = {
    // built-in core functions
    { MP_QSTR___build_class__, (mp_obj_t)&mp_builtin___build_class___obj },
    { MP_QSTR___import__, (mp_obj_t)&mp_builtin___import___obj },
    { MP_QSTR___repl_print__, (mp_obj_t)&mp_builtin___repl_print___obj },

    // built-in types
    { MP_QSTR_bool, (mp_obj_t)&bool_type },
#if MICROPY_ENABLE_FLOAT
    { MP_QSTR_complex, (mp_obj_t)&complex_type },
#endif
    { MP_QSTR_dict, (mp_obj_t)&dict_type },
    { MP_QSTR_enumerate, (mp_obj_t)&enumerate_type },
    { MP_QSTR_filter, (mp_obj_t)&filter_type },
#if MICROPY_ENABLE_FLOAT
    { MP_QSTR_float, (mp_obj_t)&float_type },
#endif
    { MP_QSTR_int, (mp_obj_t)&int_type },
    { MP_QSTR_list, (mp_obj_t)&list_type },
    { MP_QSTR_map, (mp_obj_t)&map_type },
    { MP_QSTR_set, (mp_obj_t)&set_type },
    { MP_QSTR_super, (mp_obj_t)&super_type },
    { MP_QSTR_tuple, (mp_obj_t)&tuple_type },
    { MP_QSTR_type, (mp_obj_t)&mp_type_type },
    { MP_QSTR_zip, (mp_obj_t)&zip_type },

    { MP_QSTR_classmethod, (mp_obj_t)&mp_type_classmethod },
    { MP_QSTR_staticmethod, (mp_obj_t)&mp_type_staticmethod },

    // built-in user functions
    { MP_QSTR_abs, (mp_obj_t)&mp_builtin_abs_obj },
    { MP_QSTR_all, (mp_obj_t)&mp_builtin_all_obj },
    { MP_QSTR_any, (mp_obj_t)&mp_builtin_any_obj },
    { MP_QSTR_bytes, (mp_obj_t)&mp_builtin_bytes_obj },
    { MP_QSTR_callable, (mp_obj_t)&mp_builtin_callable_obj },
    { MP_QSTR_chr, (mp_obj_t)&mp_builtin_chr_obj },
    { MP_QSTR_dir, (mp_obj_t)&mp_builtin_dir_obj },
    { MP_QSTR_divmod, (mp_obj_t)&mp_builtin_divmod_obj },
    { MP_QSTR_eval, (mp_obj_t)&mp_builtin_eval_obj },
    { MP_QSTR_exec, (mp_obj_t)&mp_builtin_exec_obj },
    { MP_QSTR_hash, (mp_obj_t)&mp_builtin_hash_obj },
    { MP_QSTR_id, (mp_obj_t)&mp_builtin_id_obj },
    { MP_QSTR_isinstance, (mp_obj_t)&mp_builtin_isinstance_obj },
    { MP_QSTR_issubclass, (mp_obj_t)&mp_builtin_issubclass_obj },
    { MP_QSTR_iter, (mp_obj_t)&mp_builtin_iter_obj },
    { MP_QSTR_len, (mp_obj_t)&mp_builtin_len_obj },
    { MP_QSTR_max, (mp_obj_t)&mp_builtin_max_obj },
    { MP_QSTR_min, (mp_obj_t)&mp_builtin_min_obj },
    { MP_QSTR_next, (mp_obj_t)&mp_builtin_next_obj },
    { MP_QSTR_ord, (mp_obj_t)&mp_builtin_ord_obj },
    { MP_QSTR_pow, (mp_obj_t)&mp_builtin_pow_obj },
    { MP_QSTR_print, (mp_obj_t)&mp_builtin_print_obj },
    { MP_QSTR_range, (mp_obj_t)&mp_builtin_range_obj },
    { MP_QSTR_repr, (mp_obj_t)&mp_builtin_repr_obj },
    { MP_QSTR_sorted, (mp_obj_t)&mp_builtin_sorted_obj },
    { MP_QSTR_sum, (mp_obj_t)&mp_builtin_sum_obj },
    { MP_QSTR_str, (mp_obj_t)&mp_builtin_str_obj },
    { MP_QSTR_bytearray, (mp_obj_t)&mp_builtin_bytearray_obj },

    { MP_QSTR_object, (mp_obj_t)&mp_type_object },
    // built-in exceptions
    { MP_QSTR_BaseException, (mp_obj_t)&mp_type_BaseException },
    { MP_QSTR_AssertionError, (mp_obj_t)&mp_type_AssertionError },
    { MP_QSTR_AttributeError, (mp_obj_t)&mp_type_AttributeError },
    { MP_QSTR_ImportError, (mp_obj_t)&mp_type_ImportError },
    { MP_QSTR_IndentationError, (mp_obj_t)&mp_type_IndentationError },
    { MP_QSTR_IndexError, (mp_obj_t)&mp_type_IndexError },
    { MP_QSTR_KeyError, (mp_obj_t)&mp_type_KeyError },
    { MP_QSTR_NameError, (mp_obj_t)&mp_type_NameError },
    { MP_QSTR_SyntaxError, (mp_obj_t)&mp_type_SyntaxError },
    { MP_QSTR_TypeError, (mp_obj_t)&mp_type_TypeError },
    { MP_QSTR_ValueError, (mp_obj_t)&mp_type_ValueError },
    // Somehow CPython managed to have OverflowError not inherit from ValueError ;-/
    // TODO: For MICROPY_CPYTHON_COMPAT==0 use ValueError to avoid exc proliferation
    { MP_QSTR_OverflowError, (mp_obj_t)&mp_type_OverflowError },
    { MP_QSTR_OSError, (mp_obj_t)&mp_type_OSError },
    { MP_QSTR_NotImplementedError, (mp_obj_t)&mp_type_NotImplementedError },
    { MP_QSTR_StopIteration, (mp_obj_t)&mp_type_StopIteration },

    // Extra builtins as defined by a port
    MICROPY_EXTRA_BUILTINS

    { MP_QSTR_, MP_OBJ_NULL }, // end of list sentinel
};

// a good optimising compiler will inline this if necessary
STATIC void mp_map_add_qstr(mp_map_t *map, qstr qstr, mp_obj_t value) {
    mp_map_lookup(map, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
}

void rt_init(void) {
    // locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
    map_locals = map_globals = mp_map_new(1);
    mp_map_add_qstr(map_globals, MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR___main__));

    // init built-in hash table
    mp_map_init(&map_builtins, 3);

    // init loaded modules table
    mp_map_init(&map_loaded_modules, 3);

    // built-in objects
    mp_map_add_qstr(&map_builtins, MP_QSTR_Ellipsis, mp_const_ellipsis);

    mp_obj_t m_array = mp_obj_new_module(MP_QSTR_array);
    rt_store_attr(m_array, MP_QSTR_array, (mp_obj_t)&array_type);

#if MICROPY_CPYTHON_COMPAT
    // Precreate sys module, so "import sys" didn't throw exceptions.
    mp_obj_t m_sys = mp_obj_new_module(MP_QSTR_sys);
    // Avoid warning of unused var
    (void)m_sys;
#endif
    // init sys.path
    // for efficiency, left to platform-specific startup code
    //sys_path = mp_obj_new_list(0, NULL);
    //rt_store_attr(m_sys, MP_QSTR_path, sys_path);

    mp_module_micropython_init();

    // TODO: wastes one mp_code_t structure in mem
    next_unique_code_id = 1; // 0 indicates "no code"
    unique_codes_alloc = 0;
    unique_codes = NULL;

#ifdef WRITE_CODE
    fp_write_code = fopen("out-code", "wb");
#endif
}

void rt_deinit(void) {
    m_del(mp_code_t, unique_codes, unique_codes_alloc);
    mp_map_free(map_globals);
    mp_map_deinit(&map_loaded_modules);
    mp_map_deinit(&map_builtins);
#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fclose(fp_write_code);
    }
#endif
}

uint rt_get_unique_code_id(void) {
    return next_unique_code_id++;
}

STATIC void alloc_unique_codes(void) {
    if (next_unique_code_id > unique_codes_alloc) {
        DEBUG_printf("allocate more unique codes: " UINT_FMT " -> %u\n", unique_codes_alloc, next_unique_code_id);
        // increase size of unique_codes table
        unique_codes = m_renew(mp_code_t, unique_codes, unique_codes_alloc, next_unique_code_id);
        for (uint i = unique_codes_alloc; i < next_unique_code_id; i++) {
            unique_codes[i].kind = MP_CODE_NONE;
        }
        unique_codes_alloc = next_unique_code_id;
    }
}

void rt_assign_byte_code(uint unique_code_id, byte *code, uint len, int n_args, int n_locals, int n_stack, uint scope_flags, qstr *arg_names) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_BYTE;
    unique_codes[unique_code_id].scope_flags = scope_flags;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_state = n_locals + n_stack;
    unique_codes[unique_code_id].u_byte.code = code;
    unique_codes[unique_code_id].u_byte.len = len;
    unique_codes[unique_code_id].arg_names = arg_names;

    //printf("byte code: %d bytes\n", len);

#ifdef DEBUG_PRINT
    DEBUG_printf("assign byte code: id=%d code=%p len=%u n_args=%d n_locals=%d n_stack=%d\n", unique_code_id, code, len, n_args, n_locals, n_stack);
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", code[i]);
    }
    DEBUG_printf("\n");
#if MICROPY_DEBUG_PRINTERS
    mp_byte_code_print(code, len);
#endif
#endif
}

void rt_assign_native_code(uint unique_code_id, void *fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_NATIVE;
    unique_codes[unique_code_id].scope_flags = 0;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_state = 0;
    unique_codes[unique_code_id].u_native.fun = fun;

    //printf("native code: %d bytes\n", len);

#ifdef DEBUG_PRINT
    DEBUG_printf("assign native code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fwrite(fun_data, len, 1, fp_write_code);
        fflush(fp_write_code);
    }
#endif
#endif
}

void rt_assign_inline_asm_code(uint unique_code_id, void *fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_INLINE_ASM;
    unique_codes[unique_code_id].scope_flags = 0;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_state = 0;
    unique_codes[unique_code_id].u_inline_asm.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign inline asm code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fwrite(fun_data, len, 1, fp_write_code);
    }
#endif
#endif
}

int rt_is_true(mp_obj_t arg) {
    DEBUG_OP_printf("is true %p\n", arg);
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (arg == mp_const_none) {
        return 0;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        if (MP_OBJ_SMALL_INT_VALUE(arg) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        mp_obj_type_t *type = mp_obj_get_type(arg);
        if (type->unary_op != NULL) {
            mp_obj_t result = type->unary_op(RT_UNARY_OP_BOOL, arg);
            if (result != MP_OBJ_NULL) {
                return result == mp_const_true;
            }
        }

        mp_obj_t len = mp_obj_len_maybe(arg);
        if (len != MP_OBJ_NULL) {
            // obj has a length, truth determined if len != 0
            return len != MP_OBJ_NEW_SMALL_INT(0);
        } else {
            // any other obj is true per Python semantics
            return 1;
        }
    }
}

mp_obj_t rt_list_append(mp_obj_t self_in, mp_obj_t arg) {
    return mp_obj_list_append(self_in, arg);
}

mp_obj_t rt_load_const_dec(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    uint len;
    const byte* data = qstr_data(qstr, &len);
    return mp_parse_num_decimal((const char*)data, len);
}

mp_obj_t rt_load_const_str(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    return MP_OBJ_NEW_QSTR(qstr);
}

mp_obj_t rt_load_const_bytes(qstr qstr) {
    DEBUG_OP_printf("load b'%s'\n", qstr_str(qstr));
    uint len;
    const byte *data = qstr_data(qstr, &len);
    return mp_obj_new_bytes(data, len);
}

mp_obj_t rt_load_name(qstr qstr) {
    // logic: search locals, globals, builtins
    DEBUG_OP_printf("load name %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        return elem->value;
    } else {
        return rt_load_global(qstr);
    }
}

mp_obj_t rt_load_global(qstr qstr) {
    // logic: search globals, builtins
    DEBUG_OP_printf("load global %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_map_lookup(map_globals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
    if (elem == NULL) {
        elem = mp_map_lookup(&map_builtins, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
        if (elem == NULL) {
            for (const mp_builtin_elem_t *e = &builtin_table[0]; e->qstr != MP_QSTR_; e++) {
                if (e->qstr == qstr) {
                    return e->fun;
                }
            }
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_NameError, "name '%s' is not defined", qstr_str(qstr)));
        }
    }
    return elem->value;
}

mp_obj_t rt_load_build_class(void) {
    DEBUG_OP_printf("load_build_class\n");
    mp_map_elem_t *elem = mp_map_lookup(&map_builtins, MP_OBJ_NEW_QSTR(MP_QSTR___build_class__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        return elem->value;
    } else {
        return (mp_obj_t)&mp_builtin___build_class___obj;
    }
}

mp_obj_t rt_get_cell(mp_obj_t cell) {
    return mp_obj_cell_get(cell);
}

void rt_set_cell(mp_obj_t cell, mp_obj_t val) {
    mp_obj_cell_set(cell, val);
}

void rt_store_name(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store name %s <- %p\n", qstr_str(qstr), obj);
    mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = obj;
}

void rt_delete_name(qstr qstr) {
    DEBUG_OP_printf("delete name %s \n", qstr_str(qstr));
    mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}

void rt_store_global(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store global %s <- %p\n", qstr_str(qstr), obj);
    mp_map_lookup(map_globals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = obj;
}

void rt_delete_global(qstr qstr) {
    DEBUG_OP_printf("delete global %s \n", qstr_str(qstr));
    mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}

mp_obj_t rt_unary_op(int op, mp_obj_t arg) {
    DEBUG_OP_printf("unary %d %p\n", op, arg);

    if (MP_OBJ_IS_SMALL_INT(arg)) {
        mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(arg);
        switch (op) {
            case RT_UNARY_OP_BOOL: return MP_BOOL(val != 0);
            case RT_UNARY_OP_POSITIVE: break;
            case RT_UNARY_OP_NEGATIVE: val = -val; break;
            case RT_UNARY_OP_INVERT: val = ~val; break;
            default: assert(0); val = 0;
        }
        if (MP_OBJ_FITS_SMALL_INT(val)) {
            return MP_OBJ_NEW_SMALL_INT(val);
        }
        return mp_obj_new_int(val);
    } else {
        mp_obj_type_t *type = mp_obj_get_type(arg);
        if (type->unary_op != NULL) {
            mp_obj_t result = type->unary_op(op, arg);
            if (result != NULL) {
                return result;
            }
        }
        // TODO specify in error message what the operator is
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "bad operand type for unary operator: '%s'", type->name));
    }
}

mp_obj_t rt_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    DEBUG_OP_printf("binary %d %p %p\n", op, lhs, rhs);

    // TODO correctly distinguish inplace operators for mutable objects
    // lookup logic that CPython uses for +=:
    //   check for implemented +=
    //   then check for implemented +
    //   then check for implemented seq.inplace_concat
    //   then check for implemented seq.concat
    //   then fail
    // note that list does not implement + or +=, so that inplace_concat is reached first for +=

    // deal with is
    if (op == RT_BINARY_OP_IS) {
        return MP_BOOL(lhs == rhs);
    }

    // deal with == and != for all types
    if (op == RT_BINARY_OP_EQUAL || op == RT_BINARY_OP_NOT_EQUAL) {
        if (mp_obj_equal(lhs, rhs)) {
            if (op == RT_BINARY_OP_EQUAL) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        } else {
            if (op == RT_BINARY_OP_EQUAL) {
                return mp_const_false;
            } else {
                return mp_const_true;
            }
        }
    }

    // deal with exception_match for all types
    if (op == RT_BINARY_OP_EXCEPTION_MATCH) {
        // rhs must be issubclass(rhs, BaseException)
        if (mp_obj_is_exception_type(rhs)) {
            // if lhs is an instance of an exception, then extract and use its type
            if (mp_obj_is_exception_instance(lhs)) {
                lhs = mp_obj_get_type(lhs);
            }
            if (mp_obj_is_subclass(lhs, rhs)) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        }
    }

    if (MP_OBJ_IS_SMALL_INT(lhs)) {
        mp_small_int_t lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs);
        if (MP_OBJ_IS_SMALL_INT(rhs)) {
            mp_small_int_t rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs);
            switch (op) {
                case RT_BINARY_OP_OR:
                case RT_BINARY_OP_INPLACE_OR: lhs_val |= rhs_val; break;
                case RT_BINARY_OP_XOR:
                case RT_BINARY_OP_INPLACE_XOR: lhs_val ^= rhs_val; break;
                case RT_BINARY_OP_AND:
                case RT_BINARY_OP_INPLACE_AND: lhs_val &= rhs_val; break;
                case RT_BINARY_OP_LSHIFT:
                case RT_BINARY_OP_INPLACE_LSHIFT: lhs_val <<= rhs_val; break;
                case RT_BINARY_OP_RSHIFT:
                case RT_BINARY_OP_INPLACE_RSHIFT: lhs_val >>= rhs_val; break;
                case RT_BINARY_OP_ADD:
                case RT_BINARY_OP_INPLACE_ADD: lhs_val += rhs_val; break;
                case RT_BINARY_OP_SUBTRACT:
                case RT_BINARY_OP_INPLACE_SUBTRACT: lhs_val -= rhs_val; break;
                case RT_BINARY_OP_MULTIPLY:
                case RT_BINARY_OP_INPLACE_MULTIPLY: lhs_val *= rhs_val; break;
                case RT_BINARY_OP_FLOOR_DIVIDE:
                case RT_BINARY_OP_INPLACE_FLOOR_DIVIDE: lhs_val /= rhs_val; break;
    #if MICROPY_ENABLE_FLOAT
                case RT_BINARY_OP_TRUE_DIVIDE:
                case RT_BINARY_OP_INPLACE_TRUE_DIVIDE: return mp_obj_new_float((mp_float_t)lhs_val / (mp_float_t)rhs_val);
    #endif

                // TODO implement modulo as specified by Python
                case RT_BINARY_OP_MODULO:
                case RT_BINARY_OP_INPLACE_MODULO: lhs_val %= rhs_val; break;

                // TODO check for negative power, and overflow
                case RT_BINARY_OP_POWER:
                case RT_BINARY_OP_INPLACE_POWER:
                {
                    int ans = 1;
                    while (rhs_val > 0) {
                        if (rhs_val & 1) {
                            ans *= lhs_val;
                        }
                        lhs_val *= lhs_val;
                        rhs_val /= 2;
                    }
                    lhs_val = ans;
                    break;
                }
                case RT_BINARY_OP_LESS: return MP_BOOL(lhs_val < rhs_val); break;
                case RT_BINARY_OP_MORE: return MP_BOOL(lhs_val > rhs_val); break;
                case RT_BINARY_OP_LESS_EQUAL: return MP_BOOL(lhs_val <= rhs_val); break;
                case RT_BINARY_OP_MORE_EQUAL: return MP_BOOL(lhs_val >= rhs_val); break;

                default: assert(0);
            }
            // TODO: We just should make mp_obj_new_int() inline and use that
            if (MP_OBJ_FITS_SMALL_INT(lhs_val)) {
                return MP_OBJ_NEW_SMALL_INT(lhs_val);
            }
            return mp_obj_new_int(lhs_val);
#if MICROPY_ENABLE_FLOAT
        } else if (MP_OBJ_IS_TYPE(rhs, &float_type)) {
            return mp_obj_float_binary_op(op, lhs_val, rhs);
        } else if (MP_OBJ_IS_TYPE(rhs, &complex_type)) {
            return mp_obj_complex_binary_op(op, lhs_val, 0, rhs);
#endif
        }
    }

    /* deal with `in`
     *
     * NOTE `a in b` is `b.__contains__(a)`, hence why the generic dispatch
     * needs to go below with swapped arguments
     */
    if (op == RT_BINARY_OP_IN) {
        mp_obj_type_t *type = mp_obj_get_type(rhs);
        if (type->binary_op != NULL) {
            mp_obj_t res = type->binary_op(op, rhs, lhs);
            if (res != MP_OBJ_NULL) {
                return res;
            }
        }
        if (type->getiter != NULL) {
            /* second attempt, walk the iterator */
            mp_obj_t next = NULL;
            mp_obj_t iter = rt_getiter(rhs);
            while ((next = rt_iternext(iter)) != mp_const_stop_iteration) {
                if (mp_obj_equal(next, lhs)) {
                    return mp_const_true;
                }
            }
            return mp_const_false;
        }

        nlr_jump(mp_obj_new_exception_msg_varg(
                     &mp_type_TypeError, "'%s' object is not iterable",
                     mp_obj_get_type_str(rhs)));
        return mp_const_none;
    }

    // generic binary_op supplied by type
    mp_obj_type_t *type = mp_obj_get_type(lhs);
    if (type->binary_op != NULL) {
        mp_obj_t result = type->binary_op(op, lhs, rhs);
        if (result != MP_OBJ_NULL) {
            return result;
        }
    }

    // TODO implement dispatch for reverse binary ops

    // TODO specify in error message what the operator is
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
        "unsupported operand types for binary operator: '%s', '%s'",
        mp_obj_get_type_str(lhs), mp_obj_get_type_str(rhs)));
    return mp_const_none;
}

mp_obj_t rt_make_function_from_id(int unique_code_id, mp_obj_t def_args) {
    DEBUG_OP_printf("make_function_from_id %d\n", unique_code_id);
    if (unique_code_id < 1 || unique_code_id >= next_unique_code_id) {
        // illegal code id
        return mp_const_none;
    }

    // make the function, depending on the code kind
    mp_code_t *c = &unique_codes[unique_code_id];
    mp_obj_t fun;
    switch (c->kind) {
        case MP_CODE_BYTE:
            fun = mp_obj_new_fun_bc(c->scope_flags, c->arg_names, c->n_args, def_args, c->n_state, c->u_byte.code);
            break;
        case MP_CODE_NATIVE:
            fun = rt_make_function_n(c->n_args, c->u_native.fun);
            break;
        case MP_CODE_INLINE_ASM:
            fun = mp_obj_new_fun_asm(c->n_args, c->u_inline_asm.fun);
            break;
        default:
            assert(0);
            fun = mp_const_none;
    }

    // check for generator functions and if so wrap in generator object
    if ((c->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
        fun = mp_obj_new_gen_wrap(fun);
    }

    return fun;
}

mp_obj_t rt_make_closure_from_id(int unique_code_id, mp_obj_t closure_tuple) {
    DEBUG_OP_printf("make_closure_from_id %d\n", unique_code_id);
    // make function object
    mp_obj_t ffun = rt_make_function_from_id(unique_code_id, MP_OBJ_NULL);
    // wrap function in closure object
    return mp_obj_new_closure(ffun, closure_tuple);
}

mp_obj_t rt_call_function_0(mp_obj_t fun) {
    return rt_call_function_n_kw(fun, 0, 0, NULL);
}

mp_obj_t rt_call_function_1(mp_obj_t fun, mp_obj_t arg) {
    return rt_call_function_n_kw(fun, 1, 0, &arg);
}

mp_obj_t rt_call_function_2(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2) {
    mp_obj_t args[2];
    args[0] = arg1;
    args[1] = arg2;
    return rt_call_function_n_kw(fun, 2, 0, args);
}

// wrapper that accepts n_args and n_kw in one argument
// native emitter can only pass at most 3 arguments to a function
mp_obj_t rt_call_function_n_kw_for_native(mp_obj_t fun_in, uint n_args_kw, const mp_obj_t *args) {
    return rt_call_function_n_kw(fun_in, n_args_kw & 0xff, (n_args_kw >> 8) & 0xff, args);
}

// args contains, eg: arg0  arg1  key0  value0  key1  value1
mp_obj_t rt_call_function_n_kw(mp_obj_t fun_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO improve this: fun object can specify its type and we parse here the arguments,
    // passing to the function arrays of fixed and keyword arguments

    DEBUG_OP_printf("calling function %p(n_args=%d, n_kw=%d, args=%p)\n", fun_in, n_args, n_kw, args);

    // get the type
    mp_obj_type_t *type = mp_obj_get_type(fun_in);

    // do the call
    if (type->call != NULL) {
        return type->call(fun_in, n_args, n_kw, args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not callable", type->name));
    }
}

// args contains, eg: arg0  arg1  key0  value0  key1  value1 vargs kwargs
mp_obj_t rt_call_function_n_var_kw(mp_obj_t fun_in, uint n_args, uint n_kw, uint flags, const mp_obj_t *args) {
    DEBUG_OP_printf("calling function_var_kw %p(n_args=%d, n_kw=%d, args=%p)\n", fun_in, n_args, n_kw, args);

    mp_obj_type_t *type = mp_obj_get_type(fun_in);
    if (type->call != NULL) {
        uint new_args_size = n_args + n_kw * 2; 
        uint new_n_args = n_args;
        uint new_n_kw = n_kw;       
        if (flags & MP_CALL_FLAG_VAR) {
            const mp_obj_t *vargs = args + n_args + n_kw * 2;
            int len = mp_obj_int_get(mp_obj_len_maybe(*vargs));
            DEBUG_OP_printf("vargs len=%d vargs=%p\n", len, vargs);

            new_args_size += len;

            new_n_args += len;
        }
        if (flags & MP_CALL_FLAG_KW) {
            const mp_obj_t *kwargs = args + n_args + n_kw * 2 + 1;
            int len = mp_obj_int_get(mp_obj_len_maybe(*kwargs));
            DEBUG_OP_printf("vargs len=%d kwargs=%p\n", len, kwargs);

            new_args_size += (len * 2);
            new_n_kw += len;
        }

        mp_obj_t *new_args = malloc(new_args_size * sizeof(mp_obj_t));

        for (int i = new_args_size - 1; i >= 0; i--) {
            new_args[i] = MP_OBJ_NULL;
        }

        memcpy(new_args, args, sizeof(mp_obj_t) * n_args);

        if (flags & MP_CALL_FLAG_VAR) {
            mp_obj_t *seq_items;
            uint seq_len;
            const mp_obj_t *vargs = args + n_args + n_kw * 2;
            if (MP_OBJ_IS_TYPE(vargs, &tuple_type)) {
                mp_obj_tuple_get(*vargs, &seq_len, &seq_items);
            } else {
                mp_obj_list_get(*vargs, &seq_len, &seq_items);
            }
            for (uint i = 0; i < seq_len; i++) {
                new_args[n_args+i] = seq_items[i];
            }
        }
        if (flags & MP_CALL_FLAG_KW) {

        }

        return type->call(fun_in, new_n_args, new_n_kw, new_args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not callable", type->name));
    }
}

// args contains: fun  self/NULL  arg(0)  ...  arg(n_args-2)  arg(n_args-1)  kw_key(0)  kw_val(0)  ... kw_key(n_kw-1)  kw_val(n_kw-1)
// if n_args==0 and n_kw==0 then there are only fun and self/NULL
mp_obj_t rt_call_method_n_kw(uint n_args, uint n_kw, const mp_obj_t *args) {
    DEBUG_OP_printf("call method (fun=%p, self=%p, n_args=%u, n_kw=%u, args=%p)\n", args[0], args[1], n_args, n_kw, args);
    int adjust = (args[1] == NULL) ? 0 : 1;
    return rt_call_function_n_kw(args[0], n_args + adjust, n_kw, args + 2 - adjust);
}

mp_obj_t rt_build_tuple(int n_args, mp_obj_t *items) {
    return mp_obj_new_tuple(n_args, items);
}

mp_obj_t rt_build_list(int n_args, mp_obj_t *items) {
    return mp_obj_new_list(n_args, items);
}

mp_obj_t rt_build_set(int n_args, mp_obj_t *items) {
    return mp_obj_new_set(n_args, items);
}

mp_obj_t rt_store_set(mp_obj_t set, mp_obj_t item) {
    mp_obj_set_store(set, item);
    return set;
}

// unpacked items are stored in reverse order into the array pointed to by items
void rt_unpack_sequence(mp_obj_t seq_in, uint num, mp_obj_t *items) {
    uint seq_len;
    if (MP_OBJ_IS_TYPE(seq_in, &tuple_type) || MP_OBJ_IS_TYPE(seq_in, &list_type)) {
        mp_obj_t *seq_items;
        if (MP_OBJ_IS_TYPE(seq_in, &tuple_type)) {
            mp_obj_tuple_get(seq_in, &seq_len, &seq_items);
        } else {
            mp_obj_list_get(seq_in, &seq_len, &seq_items);
        }
        if (seq_len < num) {
            goto too_short;
        } else if (seq_len > num) {
            goto too_long;
        }
        for (uint i = 0; i < num; i++) {
            items[i] = seq_items[num - 1 - i];
        }
    } else {
        mp_obj_t iterable = rt_getiter(seq_in);

        for (seq_len = 0; seq_len < num; seq_len++) {
            mp_obj_t el = rt_iternext(iterable);
            if (el == mp_const_stop_iteration) {
                goto too_short;
            }
            items[num - 1 - seq_len] = el;
        }
        if (rt_iternext(iterable) != mp_const_stop_iteration) {
            goto too_long;
        }
    }
    return;

too_short:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "need more than %d values to unpack", seq_len));
too_long:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "too many values to unpack (expected %d)", num));
}

mp_obj_t rt_build_map(int n_args) {
    return mp_obj_new_dict(n_args);
}

mp_obj_t rt_store_map(mp_obj_t map, mp_obj_t key, mp_obj_t value) {
    // map should always be a dict
    return mp_obj_dict_store(map, key, value);
}

mp_obj_t rt_load_attr(mp_obj_t base, qstr attr) {
    DEBUG_OP_printf("load attr %p.%s\n", base, qstr_str(attr));
    // use load_method
    mp_obj_t dest[2];
    rt_load_method(base, attr, dest);
    if (dest[1] == MP_OBJ_NULL) {
        // load_method returned just a normal attribute
        return dest[0];
    } else {
        // load_method returned a method, so build a bound method object
        return mp_obj_new_bound_meth(dest[0], dest[1]);
    }
}

// no attribute found, returns:     dest[0] == MP_OBJ_NULL, dest[1] == MP_OBJ_NULL
// normal attribute found, returns: dest[0] == <attribute>, dest[1] == MP_OBJ_NULL
// method attribute found, returns: dest[0] == <method>,    dest[1] == <self>
STATIC void rt_load_method_maybe(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    // clear output to indicate no attribute/method found yet
    dest[0] = MP_OBJ_NULL;
    dest[1] = MP_OBJ_NULL;

    // get the type
    mp_obj_type_t *type = mp_obj_get_type(base);

    // if this type can do its own load, then call it
    if (type->load_attr != NULL) {
        type->load_attr(base, attr, dest);
    }

    // if nothing found yet, look for built-in and generic names
    if (dest[0] == MP_OBJ_NULL) {
        if (attr == MP_QSTR___class__) {
            // a.__class__ is equivalent to type(a)
            dest[0] = type;
        } else if (attr == MP_QSTR___next__ && type->iternext != NULL) {
            dest[0] = (mp_obj_t)&mp_builtin_next_obj;
            dest[1] = base;
        } else if (type->load_attr == NULL) {
            // generic method lookup if type didn't provide a specific one
            // this is a lookup in the object (ie not class or type)
            const mp_method_t *meth = type->methods;
            if (meth != NULL) {
                for (; meth->name != NULL; meth++) {
                    if (strcmp(meth->name, qstr_str(attr)) == 0) {
                        // check if the methods are functions, static or class methods
                        // see http://docs.python.org/3.3/howto/descriptor.html
                        if (MP_OBJ_IS_TYPE(meth->fun, &mp_type_staticmethod)) {
                            // return just the function
                            dest[0] = ((mp_obj_static_class_method_t*)meth->fun)->fun;
                        } else if (MP_OBJ_IS_TYPE(meth->fun, &mp_type_classmethod)) {
                            // return a bound method, with self being the type of this object
                            dest[0] = ((mp_obj_static_class_method_t*)meth->fun)->fun;
                            dest[1] = mp_obj_get_type(base);
                        } else {
                            // return a bound method, with self being this object
                            dest[0] = (mp_obj_t)meth->fun;
                            dest[1] = base;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void rt_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    DEBUG_OP_printf("load method %p.%s\n", base, qstr_str(attr));

    rt_load_method_maybe(base, attr, dest);

    if (dest[0] == MP_OBJ_NULL) {
        // no attribute/method called attr
        // following CPython, we give a more detailed error message for type objects
        if (MP_OBJ_IS_TYPE(base, &mp_type_type)) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError, "type object '%s' has no attribute '%s'", ((mp_obj_type_t*)base)->name, qstr_str(attr)));
        } else {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
        }
    }
}

void rt_store_attr(mp_obj_t base, qstr attr, mp_obj_t value) {
    DEBUG_OP_printf("store attr %p.%s <- %p\n", base, qstr_str(attr), value);
    mp_obj_type_t *type = mp_obj_get_type(base);
    if (type->store_attr != NULL) {
        if (type->store_attr(base, attr, value)) {
            return;
        }
    }
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
}

void rt_store_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value) {
    DEBUG_OP_printf("store subscr %p[%p] <- %p\n", base, index, value);
    if (MP_OBJ_IS_TYPE(base, &list_type)) {
        // list store
        mp_obj_list_store(base, index, value);
    } else if (MP_OBJ_IS_TYPE(base, &dict_type)) {
        // dict store
        mp_obj_dict_store(base, index, value);
    } else {
        mp_obj_type_t *type = mp_obj_get_type(base);
        if (type->store_item != NULL) {
            bool r = type->store_item(base, index, value);
            if (r) {
                return;
            }
            // TODO: call base classes here?
        }
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object does not support item assignment", mp_obj_get_type_str(base)));
    }
}

mp_obj_t rt_getiter(mp_obj_t o_in) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->getiter != NULL) {
        return type->getiter(o_in);
    } else {
        // check for __getitem__ method
        mp_obj_t dest[2];
        rt_load_method_maybe(o_in, MP_QSTR___getitem__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // __getitem__ exists, create an iterator
            return mp_obj_new_getitem_iter(dest);
        } else {
            // object not iterable
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not iterable", type->name));
        }
    }
}

mp_obj_t rt_iternext(mp_obj_t o_in) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->iternext != NULL) {
        return type->iternext(o_in);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not an iterator", type->name));
    }
}

mp_obj_t rt_make_raise_obj(mp_obj_t o) {
    DEBUG_printf("raise %p\n", o);
    if (mp_obj_is_exception_type(o)) {
        // o is an exception type (it is derived from BaseException (or is BaseException))
        // create and return a new exception instance by calling o
        // TODO could have an option to disable traceback, then builtin exceptions (eg TypeError)
        // could have const instances in ROM which we return here instead
        return rt_call_function_n_kw(o, 0, 0, NULL);
    } else if (mp_obj_is_exception_instance(o)) {
        // o is an instance of an exception, so use it as the exception
        return o;
    } else {
        // o cannot be used as an exception, so return a type error (which will be raised by the caller)
        return mp_obj_new_exception_msg(&mp_type_TypeError, "exceptions must derive from BaseException");
    }
}

mp_obj_t rt_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level) {
    DEBUG_printf("import name %s\n", qstr_str(name));

    // build args array
    mp_obj_t args[5];
    args[0] = MP_OBJ_NEW_QSTR(name);
    args[1] = mp_const_none; // TODO should be globals
    args[2] = mp_const_none; // TODO should be locals
    args[3] = fromlist;
    args[4] = level; // must be 0; we don't yet support other values

    // TODO lookup __import__ and call that instead of going straight to builtin implementation
    return mp_builtin___import__(5, args);
}

mp_obj_t rt_import_from(mp_obj_t module, qstr name) {
    DEBUG_printf("import from %p %s\n", module, qstr_str(name));

    mp_obj_t x = rt_load_attr(module, name);
    /* TODO convert AttributeError to ImportError
    if (fail) {
        (ImportError, "cannot import name %s", qstr_str(name), NULL)
    }
    */
    return x;
}

void rt_import_all(mp_obj_t module) {
    DEBUG_printf("import all %p\n", module);

    mp_map_t *map = mp_obj_module_get_globals(module);
    for (uint i = 0; i < map->alloc; i++) {
        if (map->table[i].key != MP_OBJ_NULL) {
            rt_store_name(MP_OBJ_QSTR_VALUE(map->table[i].key), map->table[i].value);
        }
    }
}

mp_map_t *rt_locals_get(void) {
    return map_locals;
}

void rt_locals_set(mp_map_t *m) {
    DEBUG_OP_printf("rt_locals_set(%p)\n", m);
    map_locals = m;
}

mp_map_t *rt_globals_get(void) {
    return map_globals;
}

void rt_globals_set(mp_map_t *m) {
    DEBUG_OP_printf("rt_globals_set(%p)\n", m);
    map_globals = m;
}

mp_map_t *rt_loaded_modules_get(void) {
    return &map_loaded_modules;
}

// these must correspond to the respective enum
void *const rt_fun_table[RT_F_NUMBER_OF] = {
    rt_load_const_dec,
    rt_load_const_str,
    rt_load_name,
    rt_load_global,
    rt_load_build_class,
    rt_load_attr,
    rt_load_method,
    rt_store_name,
    rt_store_attr,
    rt_store_subscr,
    rt_is_true,
    rt_unary_op,
    rt_binary_op,
    rt_build_tuple,
    rt_build_list,
    rt_list_append,
    rt_build_map,
    rt_store_map,
    rt_build_set,
    rt_store_set,
    rt_make_function_from_id,
    rt_call_function_n_kw_for_native,
    rt_call_method_n_kw,
    rt_getiter,
    rt_iternext,
};

/*
void rt_f_vector(rt_fun_kind_t fun_kind) {
    (rt_f_table[fun_kind])();
}
*/
