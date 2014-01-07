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
#include "mpqstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_CODE (1)
#define DEBUG_printf(args...) printf(args)
#define DEBUG_OP_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#define DEBUG_OP_printf(args...) (void)0
#endif

// locals and globals need to be pointers because they can be the same in outer module scope
static mp_map_t *map_locals;
static mp_map_t *map_globals;
static mp_map_t map_builtins;

typedef enum {
    MP_CODE_NONE,
    MP_CODE_BYTE,
    MP_CODE_NATIVE,
    MP_CODE_INLINE_ASM,
} mp_code_kind_t;

typedef struct _mp_code_t {
    mp_code_kind_t kind;
    int n_args;
    int n_locals;
    int n_stack;
    bool is_generator;
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
} mp_code_t;

static int next_unique_code_id;
static machine_uint_t unique_codes_alloc = 0;
static mp_code_t *unique_codes = NULL;

#ifdef WRITE_CODE
FILE *fp_write_code = NULL;
#endif

void rt_init(void) {
    // locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
    map_locals = map_globals = mp_map_new(MP_MAP_QSTR, 1);
    mp_qstr_map_lookup(map_globals, MP_QSTR___name__, true)->value = mp_obj_new_str(MP_QSTR___main__);

    // init built-in hash table
    mp_map_init(&map_builtins, MP_MAP_QSTR, 3);

    // built-in exceptions (TODO, make these proper classes)
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_AttributeError, true)->value = mp_obj_new_exception(MP_QSTR_AttributeError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_IndexError, true)->value = mp_obj_new_exception(MP_QSTR_IndexError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_KeyError, true)->value = mp_obj_new_exception(MP_QSTR_KeyError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_NameError, true)->value = mp_obj_new_exception(MP_QSTR_NameError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_TypeError, true)->value = mp_obj_new_exception(MP_QSTR_TypeError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_SyntaxError, true)->value = mp_obj_new_exception(MP_QSTR_SyntaxError);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_ValueError, true)->value = mp_obj_new_exception(MP_QSTR_ValueError);

    // built-in objects
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_Ellipsis, true)->value = mp_const_ellipsis;

    // built-in core functions
    mp_qstr_map_lookup(&map_builtins, MP_QSTR___build_class__, true)->value = rt_make_function_2(mp_builtin___build_class__);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR___repl_print__, true)->value = rt_make_function_1(mp_builtin___repl_print__);

    // built-in types
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_bool, true)->value = (mp_obj_t)&bool_type;
#if MICROPY_ENABLE_FLOAT
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_complex, true)->value = (mp_obj_t)&complex_type;
#endif
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_dict, true)->value = (mp_obj_t)&dict_type;
#if MICROPY_ENABLE_FLOAT
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_float, true)->value = (mp_obj_t)&float_type;
#endif
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_int, true)->value = (mp_obj_t)&int_type;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_list, true)->value = (mp_obj_t)&list_type;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_set, true)->value = (mp_obj_t)&set_type;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_tuple, true)->value = (mp_obj_t)&tuple_type;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_type, true)->value = (mp_obj_t)&mp_builtin_type_obj; // TODO

    // built-in user functions; TODO covert all to &mp_builtin_xxx's
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_abs, true)->value = rt_make_function_1(mp_builtin_abs);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_all, true)->value = rt_make_function_1(mp_builtin_all);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_any, true)->value = rt_make_function_1(mp_builtin_any);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_callable, true)->value = rt_make_function_1(mp_builtin_callable);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_chr, true)->value = rt_make_function_1(mp_builtin_chr);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_divmod, true)->value = rt_make_function_2(mp_builtin_divmod);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_hash, true)->value = (mp_obj_t)&mp_builtin_hash_obj;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_iter, true)->value = (mp_obj_t)&mp_builtin_iter_obj;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_len, true)->value = rt_make_function_1(mp_builtin_len);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_max, true)->value = rt_make_function_var(1, mp_builtin_max);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_min, true)->value = rt_make_function_var(1, mp_builtin_min);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_next, true)->value = (mp_obj_t)&mp_builtin_next_obj;
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_ord, true)->value = rt_make_function_1(mp_builtin_ord);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_pow, true)->value = rt_make_function_var(2, mp_builtin_pow);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_print, true)->value = rt_make_function_var(0, mp_builtin_print);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_range, true)->value = rt_make_function_var(1, mp_builtin_range);
    mp_qstr_map_lookup(&map_builtins, MP_QSTR_sum, true)->value = rt_make_function_var(1, mp_builtin_sum);

    next_unique_code_id = 1; // 0 indicates "no code"
    unique_codes_alloc = 0;
    unique_codes = NULL;

#ifdef WRITE_CODE
    fp_write_code = fopen("out-code", "wb");
#endif
}

void rt_deinit(void) {
    m_del(mp_code_t, unique_codes, unique_codes_alloc);
#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fclose(fp_write_code);
    }
#endif
}

int rt_get_unique_code_id(void) {
    return next_unique_code_id++;
}

static void alloc_unique_codes(void) {
    if (next_unique_code_id > unique_codes_alloc) {
        // increase size of unique_codes table
        unique_codes = m_renew(mp_code_t, unique_codes, unique_codes_alloc, next_unique_code_id);
        for (int i = unique_codes_alloc; i < next_unique_code_id; i++) {
            unique_codes[i].kind = MP_CODE_NONE;
        }
        unique_codes_alloc = next_unique_code_id;
    }
}

void rt_assign_byte_code(int unique_code_id, byte *code, uint len, int n_args, int n_locals, int n_stack, bool is_generator) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_BYTE;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_locals = n_locals;
    unique_codes[unique_code_id].n_stack = n_stack;
    unique_codes[unique_code_id].is_generator = is_generator;
    unique_codes[unique_code_id].u_byte.code = code;
    unique_codes[unique_code_id].u_byte.len = len;

    //printf("byte code: %d bytes\n", len);

#ifdef DEBUG_PRINT
    DEBUG_printf("assign byte code: id=%d code=%p len=%u n_args=%d\n", unique_code_id, code, len, n_args);
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", code[i]);
    }
    DEBUG_printf("\n");
    extern void mp_show_byte_code(const byte *code, int len);
    mp_show_byte_code(code, len);

#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fwrite(code, len, 1, fp_write_code);
        fflush(fp_write_code);
    }
#endif
#endif
}

void rt_assign_native_code(int unique_code_id, void *fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_NATIVE;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_locals = 0;
    unique_codes[unique_code_id].n_stack = 0;
    unique_codes[unique_code_id].is_generator = false;
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

void rt_assign_inline_asm_code(int unique_code_id, void *fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_INLINE_ASM;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].n_locals = 0;
    unique_codes[unique_code_id].n_stack = 0;
    unique_codes[unique_code_id].is_generator = false;
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

static bool fit_small_int(mp_small_int_t o) {
    return true;
}

int rt_is_true(mp_obj_t arg) {
    DEBUG_OP_printf("is true %p\n", arg);
    if (MP_OBJ_IS_SMALL_INT(arg)) {
        if (MP_OBJ_SMALL_INT_VALUE(arg) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if (arg == mp_const_none) {
        return 0;
    } else if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else {
        assert(0);
        return 0;
    }
}

mp_obj_t rt_list_append(mp_obj_t self_in, mp_obj_t arg) {
    return mp_obj_list_append(self_in, arg);
}

#define PARSE_DEC_IN_INTG (1)
#define PARSE_DEC_IN_FRAC (2)
#define PARSE_DEC_IN_EXP  (3)

mp_obj_t rt_load_const_dec(qstr qstr) {
#if MICROPY_ENABLE_FLOAT
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    const char *s = qstr_str(qstr);
    int in = PARSE_DEC_IN_INTG;
    mp_float_t dec_val = 0;
    bool exp_neg = false;
    int exp_val = 0;
    int exp_extra = 0;
    bool imag = false;
    for (; *s; s++) {
        int dig = *s;
        if ('0' <= dig && dig <= '9') {
            dig -= '0';
            if (in == PARSE_DEC_IN_EXP) {
                exp_val = 10 * exp_val + dig;
            } else {
                dec_val = 10 * dec_val + dig;
                if (in == PARSE_DEC_IN_FRAC) {
                    exp_extra -= 1;
                }
            }
        } else if (in == PARSE_DEC_IN_INTG && dig == '.') {
            in = PARSE_DEC_IN_FRAC;
        } else if (in != PARSE_DEC_IN_EXP && (dig == 'E' || dig == 'e')) {
            in = PARSE_DEC_IN_EXP;
            if (s[1] == '+') {
                s++;
            } else if (s[1] == '-') {
                s++;
                exp_neg = true;
            }
        } else if (dig == 'J' || dig == 'j') {
            s++;
            imag = true;
            break;
        } else {
            // unknown character
            break;
        }
    }
    if (*s != 0) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_SyntaxError, "invalid syntax for number"));
    }
    if (exp_neg) {
        exp_val = -exp_val;
    }
    exp_val += exp_extra;
    for (; exp_val > 0; exp_val--) {
        dec_val *= 10;
    }
    for (; exp_val < 0; exp_val++) {
        dec_val *= 0.1;
    }
    if (imag) {
        return mp_obj_new_complex(0, dec_val);
    } else {
        return mp_obj_new_float(dec_val);
    }
#else
    nlr_jump(mp_obj_new_exception_msg(MP_QSTR_SyntaxError, "decimal numbers not supported"));
#endif
}

mp_obj_t rt_load_const_str(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    return mp_obj_new_str(qstr);
}

mp_obj_t rt_load_name(qstr qstr) {
    // logic: search locals, globals, builtins
    DEBUG_OP_printf("load name %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_qstr_map_lookup(map_locals, qstr, false);
    if (elem == NULL) {
        elem = mp_qstr_map_lookup(map_globals, qstr, false);
        if (elem == NULL) {
            elem = mp_qstr_map_lookup(&map_builtins, qstr, false);
            if (elem == NULL) {
                nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_NameError, "name '%s' is not defined", qstr_str(qstr)));
            }
        }
    }
    return elem->value;
}

mp_obj_t rt_load_global(qstr qstr) {
    // logic: search globals, builtins
    DEBUG_OP_printf("load global %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_qstr_map_lookup(map_globals, qstr, false);
    if (elem == NULL) {
        elem = mp_qstr_map_lookup(&map_builtins, qstr, false);
        if (elem == NULL) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_NameError, "name '%s' is not defined", qstr_str(qstr)));
        }
    }
    return elem->value;
}

mp_obj_t rt_load_build_class(void) {
    DEBUG_OP_printf("load_build_class\n");
    mp_map_elem_t *elem = mp_qstr_map_lookup(&map_builtins, MP_QSTR___build_class__, false);
    if (elem == NULL) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_NameError, "name '__build_class__' is not defined"));
    }
    return elem->value;
}

mp_obj_t rt_get_cell(mp_obj_t cell) {
    return mp_obj_cell_get(cell);
}

void rt_set_cell(mp_obj_t cell, mp_obj_t val) {
    mp_obj_cell_set(cell, val);
}

void rt_store_name(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store name %s <- %p\n", qstr_str(qstr), obj);
    mp_qstr_map_lookup(map_locals, qstr, true)->value = obj;
}

void rt_store_global(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store global %s <- %p\n", qstr_str(qstr), obj);
    mp_qstr_map_lookup(map_globals, qstr, true)->value = obj;
}

mp_obj_t rt_unary_op(int op, mp_obj_t arg) {
    DEBUG_OP_printf("unary %d %p\n", op, arg);
    if (MP_OBJ_IS_SMALL_INT(arg)) {
        mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(arg);
        switch (op) {
            case RT_UNARY_OP_NOT: if (val != 0) { return mp_const_true;} else { return mp_const_false; }
            case RT_UNARY_OP_POSITIVE: break;
            case RT_UNARY_OP_NEGATIVE: val = -val; break;
            case RT_UNARY_OP_INVERT: val = ~val; break;
            default: assert(0); val = 0;
        }
        if (fit_small_int(val)) {
            return MP_OBJ_NEW_SMALL_INT(val);
        } else {
            // TODO make a bignum
            assert(0);
            return mp_const_none;
        }
    } else { // will be an object (small ints are caught in previous if)
        mp_obj_base_t *o = arg;
        if (o->type->unary_op != NULL) {
            mp_obj_t result = o->type->unary_op(op, arg);
            if (result != NULL) {
                return result;
            }
        }
        // TODO specify in error message what the operator is
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "bad operand type for unary operator: '%s'", o->type->name));
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

                default: assert(0);
            }
            if (fit_small_int(lhs_val)) {
                return MP_OBJ_NEW_SMALL_INT(lhs_val);
            }
        } else if (MP_OBJ_IS_TYPE(rhs, &float_type)) {
            return mp_obj_float_binary_op(op, lhs_val, rhs);
        } else if (MP_OBJ_IS_TYPE(rhs, &complex_type)) {
            return mp_obj_complex_binary_op(op, lhs_val, 0, rhs);
        }
    } else if (MP_OBJ_IS_OBJ(lhs)) {
        mp_obj_base_t *o = lhs;
        if (o->type->binary_op != NULL) {
            mp_obj_t result = o->type->binary_op(op, lhs, rhs);
            if (result != NULL) {
                return result;
            }
        }
    }

    // TODO specify in error message what the operator is
    nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "unsupported operand type for binary operator: '%s'", mp_obj_get_type_str(lhs)));
}

mp_obj_t rt_compare_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    DEBUG_OP_printf("compare %d %p %p\n", op, lhs, rhs);

    // deal with == and !=
    if (op == RT_COMPARE_OP_EQUAL || op == RT_COMPARE_OP_NOT_EQUAL) {
        if (mp_obj_equal(lhs, rhs)) {
            if (op == RT_COMPARE_OP_EQUAL) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        } else {
            if (op == RT_COMPARE_OP_EQUAL) {
                return mp_const_false;
            } else {
                return mp_const_true;
            }
        }
    }

    // deal with exception_match
    if (op == RT_COMPARE_OP_EXCEPTION_MATCH) {
        // TODO properly! at the moment it just compares the exception identifier for equality
        if (MP_OBJ_IS_TYPE(lhs, &exception_type) && MP_OBJ_IS_TYPE(rhs, &exception_type)) {
            if (mp_obj_exception_get_type(lhs) == mp_obj_exception_get_type(rhs)) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        }
    }

    // deal with small ints
    if (MP_OBJ_IS_SMALL_INT(lhs) && MP_OBJ_IS_SMALL_INT(rhs)) {
        mp_small_int_t lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs);
        mp_small_int_t rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs);
        int cmp;
        switch (op) {
            case RT_COMPARE_OP_LESS: cmp = lhs_val < rhs_val; break;
            case RT_COMPARE_OP_MORE: cmp = lhs_val > rhs_val; break;
            case RT_COMPARE_OP_LESS_EQUAL: cmp = lhs_val <= rhs_val; break;
            case RT_COMPARE_OP_MORE_EQUAL: cmp = lhs_val >= rhs_val; break;
            default: assert(0); cmp = 0;
        }
        if (cmp) {
            return mp_const_true;
        } else {
            return mp_const_false;
        }
    }

#if MICROPY_ENABLE_FLOAT
    // deal with floats
    if (MP_OBJ_IS_TYPE(lhs, &float_type) || MP_OBJ_IS_TYPE(rhs, &float_type)) {
        mp_float_t lhs_val = mp_obj_get_float(lhs);
        mp_float_t rhs_val = mp_obj_get_float(rhs);
        int cmp;
        switch (op) {
            case RT_COMPARE_OP_LESS: cmp = lhs_val < rhs_val; break;
            case RT_COMPARE_OP_MORE: cmp = lhs_val > rhs_val; break;
            case RT_COMPARE_OP_LESS_EQUAL: cmp = lhs_val <= rhs_val; break;
            case RT_COMPARE_OP_MORE_EQUAL: cmp = lhs_val >= rhs_val; break;
            default: assert(0); cmp = 0;
        }
        if (cmp) {
            return mp_const_true;
        } else {
            return mp_const_false;
        }
    }
#endif

    // not implemented
    assert(0);
    return mp_const_none;
}

mp_obj_t rt_make_function_from_id(int unique_code_id) {
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
            fun = mp_obj_new_fun_bc(c->n_args, c->n_locals + c->n_stack, c->u_byte.code);
            break;
        case MP_CODE_NATIVE:
            switch (c->n_args) {
                case 0: fun = rt_make_function_0(c->u_native.fun); break;
                case 1: fun = rt_make_function_1((mp_fun_1_t)c->u_native.fun); break;
                case 2: fun = rt_make_function_2((mp_fun_2_t)c->u_native.fun); break;
                default: assert(0); fun = mp_const_none;
            }
            break;
        case MP_CODE_INLINE_ASM:
            fun = mp_obj_new_fun_asm(c->n_args, c->u_inline_asm.fun);
            break;
        default:
            assert(0);
            fun = mp_const_none;
    }

    // check for generator functions and if so wrap in generator object
    if (c->is_generator) {
        fun = mp_obj_new_gen_wrap(c->n_locals, c->n_stack, fun);
    }

    return fun;
}

mp_obj_t rt_make_closure_from_id(int unique_code_id, mp_obj_t closure_tuple) {
    DEBUG_OP_printf("make_closure_from_id %d\n", unique_code_id);
    // make function object
    mp_obj_t ffun = rt_make_function_from_id(unique_code_id);
    // wrap function in closure object
    return mp_obj_new_closure(ffun, closure_tuple);
}

mp_obj_t rt_call_function_0(mp_obj_t fun) {
    return rt_call_function_n(fun, 0, NULL);
}

mp_obj_t rt_call_function_1(mp_obj_t fun, mp_obj_t arg) {
    return rt_call_function_n(fun, 1, &arg);
}

mp_obj_t rt_call_function_2(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2) {
    mp_obj_t args[2];
    args[1] = arg1;
    args[0] = arg2;
    return rt_call_function_n(fun, 2, args);
}

// args are in reverse order in the array
mp_obj_t rt_call_function_n(mp_obj_t fun_in, int n_args, const mp_obj_t *args) {
    // TODO improve this: fun object can specify its type and we parse here the arguments,
    // passing to the function arrays of fixed and keyword arguments

    DEBUG_OP_printf("calling function %p(n_args=%d, args=%p)\n", fun_in, n_args, args);

    if (MP_OBJ_IS_SMALL_INT(fun_in)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "'int' object is not callable"));
    } else {
        mp_obj_base_t *fun = fun_in;
        if (fun->type->call_n != NULL) {
            return fun->type->call_n(fun_in, n_args, args);
        } else {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "'%s' object is not callable", fun->type->name));
        }
    }
}

// args are in reverse order in the array; keyword arguments come first, value then key
// eg: (value1, key1, value0, key0, arg1, arg0)
mp_obj_t rt_call_function_n_kw(mp_obj_t fun_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO merge this and _n into a single, smarter thing
    DEBUG_OP_printf("calling function %p(n_args=%d, n_kw=%d, args=%p)\n", fun_in, n_args, n_kw, args);

    if (MP_OBJ_IS_SMALL_INT(fun_in)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "'int' object is not callable"));
    } else {
        mp_obj_base_t *fun = fun_in;
        if (fun->type->call_n_kw != NULL) {
            return fun->type->call_n_kw(fun_in, n_args, n_kw, args);
        } else {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "'%s' object is not callable", fun->type->name));
        }
    }
}

// args contains: arg(n_args-1)  arg(n_args-2)  ...  arg(0)  self/NULL  fun
// if n_args==0 then there are only self/NULL and fun
mp_obj_t rt_call_method_n(uint n_args, const mp_obj_t *args) {
    DEBUG_OP_printf("call method %p(self=%p, n_args=%u)\n", args[n_args + 1], args[n_args], n_args);
    return rt_call_function_n(args[n_args + 1], n_args + ((args[n_args] == NULL) ? 0 : 1), args);
}

// args contains: kw_val(n_kw-1)  kw_key(n_kw-1) ... kw_val(0)  kw_key(0)  arg(n_args-1)  arg(n_args-2)  ...  arg(0)  self/NULL  fun
mp_obj_t rt_call_method_n_kw(uint n_args, uint n_kw, const mp_obj_t *args) {
    uint n = n_args + 2 * n_kw;
    DEBUG_OP_printf("call method %p(self=%p, n_args=%u, n_kw=%u)\n", args[n + 1], args[n], n_args, n_kw);
    return rt_call_function_n_kw(args[n + 1], n_args + ((args[n] == NULL) ? 0 : 1), n_kw, args);
}

// items are in reverse order
mp_obj_t rt_build_tuple(int n_args, mp_obj_t *items) {
    return mp_obj_new_tuple_reverse(n_args, items);
}

// items are in reverse order
mp_obj_t rt_build_list(int n_args, mp_obj_t *items) {
    return mp_obj_new_list_reverse(n_args, items);
}

mp_obj_t rt_build_set(int n_args, mp_obj_t *items) {
    return mp_obj_new_set(n_args, items);
}

mp_obj_t rt_store_set(mp_obj_t set, mp_obj_t item) {
    mp_obj_set_store(set, item);
    return set;
}

// unpacked items are stored in order into the array pointed to by items
void rt_unpack_sequence(mp_obj_t seq_in, uint num, mp_obj_t *items) {
    if (MP_OBJ_IS_TYPE(seq_in, &tuple_type) || MP_OBJ_IS_TYPE(seq_in, &list_type)) {
        uint seq_len;
        mp_obj_t *seq_items;
        if (MP_OBJ_IS_TYPE(seq_in, &tuple_type)) {
            mp_obj_tuple_get(seq_in, &seq_len, &seq_items);
        } else {
            mp_obj_list_get(seq_in, &seq_len, &seq_items);
        }
        if (seq_len < num) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_ValueError, "need more than %d values to unpack", (void*)(machine_uint_t)seq_len));
        } else if (seq_len > num) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_ValueError, "too many values to unpack (expected %d)", (void*)(machine_uint_t)num));
        }
        memcpy(items, seq_items, num * sizeof(mp_obj_t));
    } else {
        // TODO call rt_getiter and extract via rt_iternext
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "'%s' object is not iterable", mp_obj_get_type_str(seq_in)));
    }
}

mp_obj_t rt_build_map(int n_args) {
    return mp_obj_new_dict(n_args);
}

mp_obj_t rt_store_map(mp_obj_t map, mp_obj_t key, mp_obj_t value) {
    // map should always be a dict
    return mp_obj_dict_store(map, key, value);
}

mp_obj_t rt_load_attr(mp_obj_t base, qstr attr) {
    DEBUG_OP_printf("load attr %s\n", qstr_str(attr));
    if (MP_OBJ_IS_TYPE(base, &class_type)) {
        mp_map_elem_t *elem = mp_qstr_map_lookup(mp_obj_class_get_locals(base), attr, false);
        if (elem == NULL) {
            // TODO what about generic method lookup?
            goto no_attr;
        }
        return elem->value;
    } else if (MP_OBJ_IS_TYPE(base, &instance_type)) {
        return mp_obj_instance_load_attr(base, attr);
    } else if (MP_OBJ_IS_TYPE(base, &module_type)) {
        DEBUG_OP_printf("lookup module map %p\n", mp_obj_module_get_globals(base));
        mp_map_elem_t *elem = mp_qstr_map_lookup(mp_obj_module_get_globals(base), attr, false);
        if (elem == NULL) {
            // TODO what about generic method lookup?
            goto no_attr;
        }
        return elem->value;
    } else if (MP_OBJ_IS_OBJ(base)) {
        // generic method lookup
        mp_obj_base_t *o = base;
        const mp_method_t *meth = o->type->methods;
        if (meth != NULL) {
            for (; meth->name != NULL; meth++) {
                if (strcmp(meth->name, qstr_str(attr)) == 0) {
                    return mp_obj_new_bound_meth(base, (mp_obj_t)meth->fun);
                }
            }
        }
    }

no_attr:
    nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
}

void rt_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    DEBUG_OP_printf("load method %s\n", qstr_str(attr));
    if (MP_OBJ_IS_TYPE(base, &gen_instance_type) && attr == MP_QSTR___next__) {
        dest[1] = (mp_obj_t)&mp_builtin_next_obj;
        dest[0] = base;
        return;
    } else if (MP_OBJ_IS_TYPE(base, &instance_type)) {
        mp_obj_instance_load_method(base, attr, dest);
        return;
    } else if (MP_OBJ_IS_OBJ(base)) {
        // generic method lookup
        mp_obj_base_t *o = base;
        const mp_method_t *meth = o->type->methods;
        if (meth != NULL) {
            for (; meth->name != NULL; meth++) {
                if (strcmp(meth->name, qstr_str(attr)) == 0) {
                    dest[1] = (mp_obj_t)meth->fun;
                    dest[0] = base;
                    return;
                }
            }
        }
    }

    // no method; fallback to load_attr
    dest[1] = rt_load_attr(base, attr);
    dest[0] = NULL;
}

void rt_store_attr(mp_obj_t base, qstr attr, mp_obj_t value) {
    DEBUG_OP_printf("store attr %p.%s <- %p\n", base, qstr_str(attr), value);
    if (MP_OBJ_IS_TYPE(base, &class_type)) {
        // TODO CPython allows STORE_ATTR to a class, but is this the correct implementation?
        mp_map_t *locals = mp_obj_class_get_locals(base);
        mp_qstr_map_lookup(locals, attr, true)->value = value;
    } else if (MP_OBJ_IS_TYPE(base, &instance_type)) {
        mp_obj_instance_store_attr(base, attr, value);
    } else if (MP_OBJ_IS_TYPE(base, &module_type)) {
        // TODO CPython allows STORE_ATTR to a module, but is this the correct implementation?
        mp_map_t *globals = mp_obj_module_get_globals(base);
        mp_qstr_map_lookup(globals, attr, true)->value = value;
    } else {
        nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
    }
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
        assert(0);
    }
}

mp_obj_t rt_getiter(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "'int' object is not iterable"));
    } else {
        mp_obj_base_t *o = o_in;
        if (o->type->getiter != NULL) {
            return o->type->getiter(o_in);
        } else {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "'%s' object is not iterable", o->type->name));
        }
    }
}

mp_obj_t rt_iternext(mp_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "? 'int' object is not iterable"));
    } else {
        mp_obj_base_t *o = o_in;
        if (o->type->iternext != NULL) {
            return o->type->iternext(o_in);
        } else {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "? '%s' object is not iterable", o->type->name));
        }
    }
}

mp_obj_t rt_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level) {
    // build args array
    mp_obj_t args[5];
    args[0] = mp_obj_new_str(name);
    args[1] = mp_const_none; // TODO should be globals
    args[2] = mp_const_none; // TODO should be locals
    args[3] = fromlist;
    args[4] = level; // must be 0; we don't yet support other values

    // TODO lookup __import__ and call that instead of going straight to builtin implementation
    return mp_builtin___import__(5, args);
}

mp_obj_t rt_import_from(mp_obj_t module, qstr name) {
    mp_obj_t x = rt_load_attr(module, name);
    /* TODO convert AttributeError to ImportError
    if (fail) {
        (ImportError, "cannot import name %s", qstr_str(name), NULL)
    }
    */
    return x;
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
    rt_build_tuple,
    rt_build_list,
    rt_list_append,
    rt_build_map,
    rt_store_map,
    rt_build_set,
    rt_store_set,
    rt_make_function_from_id,
    rt_call_function_n,
    rt_call_method_n,
    rt_binary_op,
    rt_compare_op,
    rt_getiter,
    rt_iternext,
};

/*
void rt_f_vector(rt_fun_kind_t fun_kind) {
    (rt_f_table[fun_kind])();
}
*/
