/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_PY_OBJ_H
#define MICROPY_INCLUDED_PY_OBJ_H

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/mpprint.h"
#include "py/runtime0.h"

// This is the definition of the opaque MicroPython object type.
// All concrete objects have an encoding within this type and the
// particular encoding is specified by MICROPY_OBJ_REPR.
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
typedef uint64_t mp_obj_t;
typedef uint64_t mp_const_obj_t;
#else
typedef void *mp_obj_t;
typedef const void *mp_const_obj_t;
#endif

// This mp_obj_type_t struct is a concrete MicroPython object which holds info
// about a type.  See below for actual definition of the struct.
typedef struct _mp_obj_type_t mp_obj_type_t;

// Anything that wants to be a concrete MicroPython object must have mp_obj_base_t
// as its first member (small ints, qstr objs and inline floats are not concrete).
struct _mp_obj_base_t {
    const mp_obj_type_t *type MICROPY_OBJ_BASE_ALIGNMENT;
};
typedef struct _mp_obj_base_t mp_obj_base_t;

// These fake objects are used to indicate certain things in arguments or return
// values, and should only be used when explicitly allowed.
//
//  - MP_OBJ_NULL : used to indicate the absence of an object, or unsupported operation.
//  - MP_OBJ_STOP_ITERATION : used instead of throwing a StopIteration, for efficiency.
//  - MP_OBJ_SENTINEL : used for various internal purposes where one needs
//    an object which is unique from all other objects, including MP_OBJ_NULL.
//
// For debugging purposes they are all different.  For non-debug mode, we alias
// as many as we can to MP_OBJ_NULL because it's cheaper to load/compare 0.

#ifdef NDEBUG
#define MP_OBJ_NULL             (MP_OBJ_FROM_PTR((void*)0))
#define MP_OBJ_STOP_ITERATION   (MP_OBJ_FROM_PTR((void*)0))
#define MP_OBJ_SENTINEL         (MP_OBJ_FROM_PTR((void*)4))
#else
#define MP_OBJ_NULL             (MP_OBJ_FROM_PTR((void*)0))
#define MP_OBJ_STOP_ITERATION   (MP_OBJ_FROM_PTR((void*)4))
#define MP_OBJ_SENTINEL         (MP_OBJ_FROM_PTR((void*)8))
#endif

// These macros/inline functions operate on objects and depend on the
// particular object representation.  They are used to query, pack and
// unpack small ints, qstrs and full object pointers.

#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A

static inline bool MP_OBJ_IS_SMALL_INT(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 1) != 0); }
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 1) | 1))

static inline bool MP_OBJ_IS_QSTR(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 3) == 2); }
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 2)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 2) | 2))

#if MICROPY_PY_BUILTINS_FLOAT
#define mp_const_float_e MP_ROM_PTR(&mp_const_float_e_obj)
#define mp_const_float_pi MP_ROM_PTR(&mp_const_float_pi_obj)
extern const struct _mp_obj_float_t mp_const_float_e_obj;
extern const struct _mp_obj_float_t mp_const_float_pi_obj;

#define mp_obj_is_float(o) MP_OBJ_IS_TYPE((o), &mp_type_float)
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_new_float(mp_float_t value);
#endif

static inline bool MP_OBJ_IS_OBJ(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 3) == 0); }

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B

static inline bool MP_OBJ_IS_SMALL_INT(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 3) == 1); }
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 2)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 2) | 1))

static inline bool MP_OBJ_IS_QSTR(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 3) == 3); }
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 2)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 2) | 3))

#if MICROPY_PY_BUILTINS_FLOAT
#define mp_const_float_e MP_ROM_PTR(&mp_const_float_e_obj)
#define mp_const_float_pi MP_ROM_PTR(&mp_const_float_pi_obj)
extern const struct _mp_obj_float_t mp_const_float_e_obj;
extern const struct _mp_obj_float_t mp_const_float_pi_obj;

#define mp_obj_is_float(o) MP_OBJ_IS_TYPE((o), &mp_type_float)
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_new_float(mp_float_t value);
#endif

static inline bool MP_OBJ_IS_OBJ(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 1) == 0); }

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C

static inline bool MP_OBJ_IS_SMALL_INT(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 1) != 0); }
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 1) | 1))

#define mp_const_float_e MP_ROM_PTR((mp_obj_t)(((0x402df854 & ~3) | 2) + 0x80800000))
#define mp_const_float_pi MP_ROM_PTR((mp_obj_t)(((0x40490fdb & ~3) | 2) + 0x80800000))

static inline bool mp_obj_is_float(mp_const_obj_t o)
    { return (((mp_uint_t)(o)) & 3) == 2 && (((mp_uint_t)(o)) & 0xff800007) != 0x00000006; }
static inline mp_float_t mp_obj_float_get(mp_const_obj_t o) {
    union {
        mp_float_t f;
        mp_uint_t u;
    } num = {.u = ((mp_uint_t)o - 0x80800000) & ~3};
    return num.f;
}
static inline mp_obj_t mp_obj_new_float(mp_float_t f) {
    union {
        mp_float_t f;
        mp_uint_t u;
    } num = {.f = f};
    return (mp_obj_t)(((num.u & ~0x3) | 2) + 0x80800000);
}

static inline bool MP_OBJ_IS_QSTR(mp_const_obj_t o)
    { return (((mp_uint_t)(o)) & 0xff800007) == 0x00000006; }
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 3)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 3) | 0x00000006))

static inline bool MP_OBJ_IS_OBJ(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 3) == 0); }

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D

static inline bool MP_OBJ_IS_SMALL_INT(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 0xffff000000000000) == 0x0001000000000000); }
#define MP_OBJ_SMALL_INT_VALUE(o) (((intptr_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)(((uintptr_t)(small_int)) << 1) | 0x0001000000000001)

static inline bool MP_OBJ_IS_QSTR(mp_const_obj_t o)
    { return ((((mp_int_t)(o)) & 0xffff000000000000) == 0x0002000000000000); }
#define MP_OBJ_QSTR_VALUE(o) ((((uint32_t)(o)) >> 1) & 0xffffffff)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 1) | 0x0002000000000001))

#if MICROPY_PY_BUILTINS_FLOAT
#define mp_const_float_e {((mp_obj_t)((uint64_t)0x4005bf0a8b125769 + 0x8004000000000000))}
#define mp_const_float_pi {((mp_obj_t)((uint64_t)0x400921fb54442d18 + 0x8004000000000000))}

static inline bool mp_obj_is_float(mp_const_obj_t o) {
    return ((uint64_t)(o) & 0xfffc000000000000) != 0;
}
static inline mp_float_t mp_obj_float_get(mp_const_obj_t o) {
    union {
        mp_float_t f;
        uint64_t r;
    } num = {.r = o - 0x8004000000000000};
    return num.f;
}
static inline mp_obj_t mp_obj_new_float(mp_float_t f) {
    union {
        mp_float_t f;
        uint64_t r;
    } num = {.f = f};
    return num.r + 0x8004000000000000;
}
#endif

static inline bool MP_OBJ_IS_OBJ(mp_const_obj_t o)
    { return ((((uint64_t)(o)) & 0xffff000000000000) == 0x0000000000000000); }
#define MP_OBJ_TO_PTR(o) ((void*)(uintptr_t)(o))
#define MP_OBJ_FROM_PTR(p) ((mp_obj_t)((uintptr_t)(p)))

// rom object storage needs special handling to widen 32-bit pointer to 64-bits
typedef union _mp_rom_obj_t { uint64_t u64; struct { const void *lo, *hi; } u32; } mp_rom_obj_t;
#define MP_ROM_INT(i) {MP_OBJ_NEW_SMALL_INT(i)}
#define MP_ROM_QSTR(q) {MP_OBJ_NEW_QSTR(q)}
#if MP_ENDIANNESS_LITTLE
#define MP_ROM_PTR(p) {.u32 = {.lo = (p), .hi = NULL}}
#else
#define MP_ROM_PTR(p) {.u32 = {.lo = NULL, .hi = (p)}}
#endif

#endif

// Macros to convert between mp_obj_t and concrete object types.
// These are identity operations in MicroPython, but ability to override
// these operations are provided to experiment with other methods of
// object representation and memory management.

// Cast mp_obj_t to object pointer
#ifndef MP_OBJ_TO_PTR
#define MP_OBJ_TO_PTR(o) ((void*)o)
#endif

// Cast object pointer to mp_obj_t
#ifndef MP_OBJ_FROM_PTR
#define MP_OBJ_FROM_PTR(p) ((mp_obj_t)p)
#endif

// Macros to create objects that are stored in ROM.

#ifndef MP_ROM_INT
typedef mp_const_obj_t mp_rom_obj_t;
#define MP_ROM_INT(i) MP_OBJ_NEW_SMALL_INT(i)
#define MP_ROM_QSTR(q) MP_OBJ_NEW_QSTR(q)
#define MP_ROM_PTR(p) (p)
/* for testing
typedef struct _mp_rom_obj_t { mp_const_obj_t o; } mp_rom_obj_t;
#define MP_ROM_INT(i) {MP_OBJ_NEW_SMALL_INT(i)}
#define MP_ROM_QSTR(q) {MP_OBJ_NEW_QSTR(q)}
#define MP_ROM_PTR(p) {.o = p}
*/
#endif

// The macros below are derived from the ones above and are used to
// check for more specific object types.

#define MP_OBJ_IS_TYPE(o, t) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type == (t))) // this does not work for checking int, str or fun; use below macros for that
#define MP_OBJ_IS_INT(o) (MP_OBJ_IS_SMALL_INT(o) || MP_OBJ_IS_TYPE(o, &mp_type_int))
#define MP_OBJ_IS_STR(o) (MP_OBJ_IS_QSTR(o) || MP_OBJ_IS_TYPE(o, &mp_type_str))
#define MP_OBJ_IS_STR_OR_BYTES(o) (MP_OBJ_IS_QSTR(o) || (MP_OBJ_IS_OBJ(o) && ((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type->binary_op == mp_obj_str_binary_op))
#define MP_OBJ_IS_FUN(o) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_function))

// Note: inline functions sometimes use much more code space than the
// equivalent macros, depending on the compiler.
//static inline bool MP_OBJ_IS_TYPE(mp_const_obj_t o, const mp_obj_type_t *t) { return (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)(o))->type == (t))); } // this does not work for checking a string, use below macro for that
//static inline bool MP_OBJ_IS_INT(mp_const_obj_t o) { return (MP_OBJ_IS_SMALL_INT(o) || MP_OBJ_IS_TYPE(o, &mp_type_int)); } // returns true if o is a small int or long int
// Need to forward declare these for the inline function to compile.
extern const mp_obj_type_t mp_type_int;
extern const mp_obj_type_t mp_type_bool;
static inline bool mp_obj_is_integer(mp_const_obj_t o) { return MP_OBJ_IS_INT(o) || MP_OBJ_IS_TYPE(o, &mp_type_bool); } // returns true if o is bool, small int or long int
//static inline bool MP_OBJ_IS_STR(mp_const_obj_t o) { return (MP_OBJ_IS_QSTR(o) || MP_OBJ_IS_TYPE(o, &mp_type_str)); }


// These macros are used to declare and define constant function objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_FUN_OBJ_0(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_1(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_2(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_3(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_VAR(obj_name) extern const mp_obj_fun_builtin_var_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name) extern const mp_obj_fun_builtin_var_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_KW(obj_name) extern const mp_obj_fun_builtin_var_t obj_name

#define MP_DEFINE_CONST_FUN_OBJ_0(obj_name, fun_name) \
    const mp_obj_fun_builtin_fixed_t obj_name = \
        {{&mp_type_fun_builtin_0}, .fun._0 = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_1(obj_name, fun_name) \
    const mp_obj_fun_builtin_fixed_t obj_name = \
        {{&mp_type_fun_builtin_1}, .fun._1 = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_2(obj_name, fun_name) \
    const mp_obj_fun_builtin_fixed_t obj_name = \
        {{&mp_type_fun_builtin_2}, .fun._2 = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_3(obj_name, fun_name) \
    const mp_obj_fun_builtin_fixed_t obj_name = \
        {{&mp_type_fun_builtin_3}, .fun._3 = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_VAR(obj_name, n_args_min, fun_name) \
    const mp_obj_fun_builtin_var_t obj_name = \
        {{&mp_type_fun_builtin_var}, false, n_args_min, MP_OBJ_FUN_ARGS_MAX, .fun.var = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name, n_args_min, n_args_max, fun_name) \
    const mp_obj_fun_builtin_var_t obj_name = \
        {{&mp_type_fun_builtin_var}, false, n_args_min, n_args_max, .fun.var = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_KW(obj_name, n_args_min, fun_name) \
    const mp_obj_fun_builtin_var_t obj_name = \
        {{&mp_type_fun_builtin_var}, true, n_args_min, MP_OBJ_FUN_ARGS_MAX, .fun.kw = fun_name}

// These macros are used to define constant map/dict objects
// You can put "static" in front of the definition to make it local

#define MP_DEFINE_CONST_MAP(map_name, table_name) \
    const mp_map_t map_name = { \
        .all_keys_are_qstrs = 1, \
        .is_fixed = 1, \
        .is_ordered = 1, \
        .used = MP_ARRAY_SIZE(table_name), \
        .alloc = MP_ARRAY_SIZE(table_name), \
        .table = (mp_map_elem_t*)(mp_rom_map_elem_t*)table_name, \
    }

#define MP_DEFINE_CONST_DICT(dict_name, table_name) \
    const mp_obj_dict_t dict_name = { \
        .base = {&mp_type_dict}, \
        .map = { \
            .all_keys_are_qstrs = 1, \
            .is_fixed = 1, \
            .is_ordered = 1, \
            .used = MP_ARRAY_SIZE(table_name), \
            .alloc = MP_ARRAY_SIZE(table_name), \
            .table = (mp_map_elem_t*)(mp_rom_map_elem_t*)table_name, \
        }, \
    }

// These macros are used to declare and define constant staticmethond and classmethod objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_STATICMETHOD_OBJ(obj_name) extern const mp_rom_obj_static_class_method_t obj_name
#define MP_DECLARE_CONST_CLASSMETHOD_OBJ(obj_name) extern const mp_rom_obj_static_class_method_t obj_name

#define MP_DEFINE_CONST_STATICMETHOD_OBJ(obj_name, fun_name) const mp_rom_obj_static_class_method_t obj_name = {{&mp_type_staticmethod}, fun_name}
#define MP_DEFINE_CONST_CLASSMETHOD_OBJ(obj_name, fun_name) const mp_rom_obj_static_class_method_t obj_name = {{&mp_type_classmethod}, fun_name}

// Underlying map/hash table implementation (not dict object or map function)

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct _mp_rom_map_elem_t {
    mp_rom_obj_t key;
    mp_rom_obj_t value;
} mp_rom_map_elem_t;

// TODO maybe have a truncated mp_map_t for fixed tables, since alloc=used
// put alloc last in the structure, so the truncated version does not need it
// this would save 1 ROM word for all ROM objects that have a locals_dict
// would also need a trucated dict structure

typedef struct _mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;    // a fixed array that can't be modified; must also be ordered
    size_t is_ordered : 1;  // an ordered array
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t *table;
} mp_map_t;

// mp_set_lookup requires these constants to have the values they do
typedef enum _mp_map_lookup_kind_t {
    MP_MAP_LOOKUP = 0,
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND = 1,
    MP_MAP_LOOKUP_REMOVE_IF_FOUND = 2,
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND_OR_REMOVE_IF_FOUND = 3, // only valid for mp_set_lookup
} mp_map_lookup_kind_t;

extern const mp_map_t mp_const_empty_map;

static inline bool MP_MAP_SLOT_IS_FILLED(const mp_map_t *map, size_t pos) { return ((map)->table[pos].key != MP_OBJ_NULL && (map)->table[pos].key != MP_OBJ_SENTINEL); }

void mp_map_init(mp_map_t *map, size_t n);
void mp_map_init_fixed_table(mp_map_t *map, size_t n, const mp_obj_t *table);
mp_map_t *mp_map_new(size_t n);
void mp_map_deinit(mp_map_t *map);
void mp_map_free(mp_map_t *map);
mp_map_elem_t *mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_map_clear(mp_map_t *map);
void mp_map_dump(mp_map_t *map);

// Underlying set implementation (not set object)

typedef struct _mp_set_t {
    size_t alloc;
    size_t used;
    mp_obj_t *table;
} mp_set_t;

static inline bool MP_SET_SLOT_IS_FILLED(const mp_set_t *set, size_t pos) { return ((set)->table[pos] != MP_OBJ_NULL && (set)->table[pos] != MP_OBJ_SENTINEL); }

void mp_set_init(mp_set_t *set, size_t n);
mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
mp_obj_t mp_set_remove_first(mp_set_t *set);
void mp_set_clear(mp_set_t *set);

// Type definitions for methods

typedef mp_obj_t (*mp_fun_0_t)(void);
typedef mp_obj_t (*mp_fun_1_t)(mp_obj_t);
typedef mp_obj_t (*mp_fun_2_t)(mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_3_t)(mp_obj_t, mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_var_t)(size_t n, const mp_obj_t *);
// mp_fun_kw_t takes mp_map_t* (and not const mp_map_t*) to ease passing
// this arg to mp_map_lookup().
typedef mp_obj_t (*mp_fun_kw_t)(size_t n, const mp_obj_t *, mp_map_t *);

typedef enum {
    PRINT_STR = 0,
    PRINT_REPR = 1,
    PRINT_EXC = 2, // Special format for printing exception in unhandled exception message
    PRINT_JSON = 3,
    PRINT_RAW = 4, // Special format for printing bytes as an undercorated string
    PRINT_EXC_SUBCLASS = 0x80, // Internal flag for printing exception subclasses
} mp_print_kind_t;

typedef struct _mp_obj_iter_buf_t {
    mp_obj_base_t base;
    mp_obj_t buf[3];
} mp_obj_iter_buf_t;

// The number of slots that an mp_obj_iter_buf_t needs on the Python value stack.
// It's rounded up in case mp_obj_base_t is smaller than mp_obj_t (eg for OBJ_REPR_D).
#define MP_OBJ_ITER_BUF_NSLOTS ((sizeof(mp_obj_iter_buf_t) + sizeof(mp_obj_t) - 1) / sizeof(mp_obj_t))

typedef void (*mp_print_fun_t)(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind);
typedef mp_obj_t (*mp_make_new_fun_t)(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_call_fun_t)(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_unary_op_fun_t)(mp_unary_op_t op, mp_obj_t);
typedef mp_obj_t (*mp_binary_op_fun_t)(mp_binary_op_t op, mp_obj_t, mp_obj_t);
typedef void (*mp_attr_fun_t)(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
typedef mp_obj_t (*mp_subscr_fun_t)(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
typedef mp_obj_t (*mp_getiter_fun_t)(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf);

// Buffer protocol
typedef struct _mp_buffer_info_t {
    // if we'd bother to support various versions of structure
    // (with different number of fields), we can distinguish
    // them with ver = sizeof(struct). Cons: overkill for *micro*?
    //int ver; // ?

    void *buf;      // can be NULL if len == 0
    size_t len;     // in bytes
    int typecode;   // as per binary.h

    // Rationale: to load arbitrary-sized sprites directly to LCD
    // Cons: a bit adhoc usecase
    // int stride;
} mp_buffer_info_t;
#define MP_BUFFER_READ  (1)
#define MP_BUFFER_WRITE (2)
#define MP_BUFFER_RW (MP_BUFFER_READ | MP_BUFFER_WRITE)
typedef struct _mp_buffer_p_t {
    mp_int_t (*get_buffer)(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
} mp_buffer_p_t;
bool mp_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
void mp_get_buffer_raise(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);

// Stream protocol
typedef struct _mp_stream_p_t {
    // On error, functions should return MP_STREAM_ERROR and fill in *errcode (values
    // are implementation-dependent, but will be exposed to user, e.g. via exception).
    mp_uint_t (*read)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    mp_uint_t (*write)(mp_obj_t obj, const void *buf, mp_uint_t size, int *errcode);
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    mp_uint_t is_text : 1; // default is bytes, set this for text stream
} mp_stream_p_t;

struct _mp_obj_type_t {
    // A type is an object so must start with this entry, which points to mp_type_type.
    mp_obj_base_t base;

    // The name of this type.
    qstr name;

    // Corresponds to __repr__ and __str__ special methods.
    mp_print_fun_t print;

    // Corresponds to __new__ and __init__ special methods, to make an instance of the type.
    mp_make_new_fun_t make_new;

    // Corresponds to __call__ special method, ie T(...).
    mp_call_fun_t call;

    // Implements unary and binary operations.
    // Can return MP_OBJ_NULL if the operation is not supported.
    mp_unary_op_fun_t unary_op;
    mp_binary_op_fun_t binary_op;

    // Implements load, store and delete attribute.
    //
    // dest[0] = MP_OBJ_NULL means load
    //  return: for fail, do nothing
    //          for attr, dest[0] = value
    //          for method, dest[0] = method, dest[1] = self
    //
    // dest[0,1] = {MP_OBJ_SENTINEL, MP_OBJ_NULL} means delete
    // dest[0,1] = {MP_OBJ_SENTINEL, object} means store
    //  return: for fail, do nothing
    //          for success set dest[0] = MP_OBJ_NULL
    mp_attr_fun_t attr;

    // Implements load, store and delete subscripting:
    //  - value = MP_OBJ_SENTINEL means load
    //  - value = MP_OBJ_NULL means delete
    //  - all other values mean store the value
    // Can return MP_OBJ_NULL if operation not supported.
    mp_subscr_fun_t subscr;

    // Corresponds to __iter__ special method.
    // Can use the given mp_obj_iter_buf_t to store iterator object,
    // otherwise can return a pointer to an object on the heap.
    mp_getiter_fun_t getiter;

    // Corresponds to __next__ special method.  May return MP_OBJ_STOP_ITERATION
    // as an optimisation instead of raising StopIteration() with no args.
    mp_fun_1_t iternext;

    // Implements the buffer protocol if supported by this type.
    mp_buffer_p_t buffer_p;

    // One of disjoint protocols (interfaces), like mp_stream_p_t, etc.
    const void *protocol;

    // A pointer to the parents of this type:
    //  - 0 parents: pointer is NULL (object is implicitly the single parent)
    //  - 1 parent: a pointer to the type of that parent
    //  - 2 or more parents: pointer to a tuple object containing the parent types
    const void *parent;

    // A dict mapping qstrs to objects local methods/constants/etc.
    struct _mp_obj_dict_t *locals_dict;
};

// Constant types, globally accessible
extern const mp_obj_type_t mp_type_type;
extern const mp_obj_type_t mp_type_object;
extern const mp_obj_type_t mp_type_NoneType;
extern const mp_obj_type_t mp_type_bool;
extern const mp_obj_type_t mp_type_int;
extern const mp_obj_type_t mp_type_str;
extern const mp_obj_type_t mp_type_bytes;
extern const mp_obj_type_t mp_type_bytearray;
extern const mp_obj_type_t mp_type_memoryview;
extern const mp_obj_type_t mp_type_float;
extern const mp_obj_type_t mp_type_complex;
extern const mp_obj_type_t mp_type_tuple;
extern const mp_obj_type_t mp_type_list;
extern const mp_obj_type_t mp_type_map; // map (the python builtin, not the dict implementation detail)
extern const mp_obj_type_t mp_type_enumerate;
extern const mp_obj_type_t mp_type_filter;
extern const mp_obj_type_t mp_type_dict;
extern const mp_obj_type_t mp_type_ordereddict;
extern const mp_obj_type_t mp_type_range;
extern const mp_obj_type_t mp_type_set;
extern const mp_obj_type_t mp_type_frozenset;
extern const mp_obj_type_t mp_type_slice;
extern const mp_obj_type_t mp_type_zip;
extern const mp_obj_type_t mp_type_array;
extern const mp_obj_type_t mp_type_super;
extern const mp_obj_type_t mp_type_gen_instance;
extern const mp_obj_type_t mp_type_fun_builtin_0;
extern const mp_obj_type_t mp_type_fun_builtin_1;
extern const mp_obj_type_t mp_type_fun_builtin_2;
extern const mp_obj_type_t mp_type_fun_builtin_3;
extern const mp_obj_type_t mp_type_fun_builtin_var;
extern const mp_obj_type_t mp_type_fun_bc;
extern const mp_obj_type_t mp_type_module;
extern const mp_obj_type_t mp_type_staticmethod;
extern const mp_obj_type_t mp_type_classmethod;
extern const mp_obj_type_t mp_type_property;
extern const mp_obj_type_t mp_type_stringio;
extern const mp_obj_type_t mp_type_bytesio;
extern const mp_obj_type_t mp_type_reversed;
extern const mp_obj_type_t mp_type_polymorph_iter;

// Exceptions
extern const mp_obj_type_t mp_type_BaseException;
extern const mp_obj_type_t mp_type_ArithmeticError;
extern const mp_obj_type_t mp_type_AssertionError;
extern const mp_obj_type_t mp_type_AttributeError;
extern const mp_obj_type_t mp_type_EOFError;
extern const mp_obj_type_t mp_type_Exception;
extern const mp_obj_type_t mp_type_GeneratorExit;
extern const mp_obj_type_t mp_type_ImportError;
extern const mp_obj_type_t mp_type_IndentationError;
extern const mp_obj_type_t mp_type_IndexError;
extern const mp_obj_type_t mp_type_KeyboardInterrupt;
extern const mp_obj_type_t mp_type_KeyError;
extern const mp_obj_type_t mp_type_LookupError;
extern const mp_obj_type_t mp_type_MemoryError;
extern const mp_obj_type_t mp_type_NameError;
extern const mp_obj_type_t mp_type_NotImplementedError;
extern const mp_obj_type_t mp_type_OSError;
extern const mp_obj_type_t mp_type_TimeoutError;
extern const mp_obj_type_t mp_type_OverflowError;
extern const mp_obj_type_t mp_type_RuntimeError;
extern const mp_obj_type_t mp_type_StopAsyncIteration;
extern const mp_obj_type_t mp_type_StopIteration;
extern const mp_obj_type_t mp_type_SyntaxError;
extern const mp_obj_type_t mp_type_SystemExit;
extern const mp_obj_type_t mp_type_TypeError;
extern const mp_obj_type_t mp_type_UnicodeError;
extern const mp_obj_type_t mp_type_ValueError;
extern const mp_obj_type_t mp_type_ViperTypeError;
extern const mp_obj_type_t mp_type_ZeroDivisionError;

// Constant objects, globally accessible
// The macros are for convenience only
#define mp_const_none (MP_OBJ_FROM_PTR(&mp_const_none_obj))
#define mp_const_false (MP_OBJ_FROM_PTR(&mp_const_false_obj))
#define mp_const_true (MP_OBJ_FROM_PTR(&mp_const_true_obj))
#define mp_const_empty_bytes (MP_OBJ_FROM_PTR(&mp_const_empty_bytes_obj))
#define mp_const_empty_tuple (MP_OBJ_FROM_PTR(&mp_const_empty_tuple_obj))
#define mp_const_notimplemented (MP_OBJ_FROM_PTR(&mp_const_notimplemented_obj))
extern const struct _mp_obj_none_t mp_const_none_obj;
extern const struct _mp_obj_bool_t mp_const_false_obj;
extern const struct _mp_obj_bool_t mp_const_true_obj;
extern const struct _mp_obj_str_t mp_const_empty_bytes_obj;
extern const struct _mp_obj_tuple_t mp_const_empty_tuple_obj;
extern const struct _mp_obj_singleton_t mp_const_ellipsis_obj;
extern const struct _mp_obj_singleton_t mp_const_notimplemented_obj;
extern const struct _mp_obj_exception_t mp_const_MemoryError_obj;
extern const struct _mp_obj_exception_t mp_const_GeneratorExit_obj;

// General API for objects

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict);
static inline mp_obj_t mp_obj_new_bool(mp_int_t x) { return x ? mp_const_true : mp_const_false; }
mp_obj_t mp_obj_new_cell(mp_obj_t obj);
mp_obj_t mp_obj_new_int(mp_int_t value);
mp_obj_t mp_obj_new_int_from_uint(mp_uint_t value);
mp_obj_t mp_obj_new_int_from_str_len(const char **str, size_t len, bool neg, unsigned int base);
mp_obj_t mp_obj_new_int_from_ll(long long val); // this must return a multi-precision integer object (or raise an overflow exception)
mp_obj_t mp_obj_new_int_from_ull(unsigned long long val); // this must return a multi-precision integer object (or raise an overflow exception)
mp_obj_t mp_obj_new_str(const char* data, size_t len);
mp_obj_t mp_obj_new_str_via_qstr(const char* data, size_t len);
mp_obj_t mp_obj_new_str_from_vstr(const mp_obj_type_t *type, vstr_t *vstr);
mp_obj_t mp_obj_new_bytes(const byte* data, size_t len);
mp_obj_t mp_obj_new_bytearray(size_t n, void *items);
mp_obj_t mp_obj_new_bytearray_by_ref(size_t n, void *items);
#if MICROPY_PY_BUILTINS_FLOAT
mp_obj_t mp_obj_new_int_from_float(mp_float_t val);
mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);
#endif
mp_obj_t mp_obj_new_exception(const mp_obj_type_t *exc_type);
mp_obj_t mp_obj_new_exception_arg1(const mp_obj_type_t *exc_type, mp_obj_t arg);
mp_obj_t mp_obj_new_exception_args(const mp_obj_type_t *exc_type, size_t n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_exception_msg(const mp_obj_type_t *exc_type, const char *msg);
mp_obj_t mp_obj_new_exception_msg_varg(const mp_obj_type_t *exc_type, const char *fmt, ...); // counts args by number of % symbols in fmt, excluding %%; can only handle void* sizes (ie no float/double!)
mp_obj_t mp_obj_new_fun_bc(mp_obj_t def_args, mp_obj_t def_kw_args, const byte *code, const mp_uint_t *const_table);
mp_obj_t mp_obj_new_fun_native(mp_obj_t def_args_in, mp_obj_t def_kw_args, const void *fun_data, const mp_uint_t *const_table);
mp_obj_t mp_obj_new_fun_viper(size_t n_args, void *fun_data, mp_uint_t type_sig);
mp_obj_t mp_obj_new_fun_asm(size_t n_args, void *fun_data, mp_uint_t type_sig);
mp_obj_t mp_obj_new_gen_wrap(mp_obj_t fun);
mp_obj_t mp_obj_new_closure(mp_obj_t fun, size_t n_closed, const mp_obj_t *closed);
mp_obj_t mp_obj_new_tuple(size_t n, const mp_obj_t *items);
mp_obj_t mp_obj_new_list(size_t n, mp_obj_t *items);
mp_obj_t mp_obj_new_dict(size_t n_args);
mp_obj_t mp_obj_new_set(size_t n_args, mp_obj_t *items);
mp_obj_t mp_obj_new_slice(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
mp_obj_t mp_obj_new_bound_meth(mp_obj_t meth, mp_obj_t self);
mp_obj_t mp_obj_new_getitem_iter(mp_obj_t *args, mp_obj_iter_buf_t *iter_buf);
mp_obj_t mp_obj_new_module(qstr module_name);
mp_obj_t mp_obj_new_memoryview(byte typecode, size_t nitems, void *items);

mp_obj_type_t *mp_obj_get_type(mp_const_obj_t o_in);
const char *mp_obj_get_type_str(mp_const_obj_t o_in);
bool mp_obj_is_subclass_fast(mp_const_obj_t object, mp_const_obj_t classinfo); // arguments should be type objects
mp_obj_t mp_instance_cast_to_native_base(mp_const_obj_t self_in, mp_const_obj_t native_type);

void mp_obj_print_helper(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
void mp_obj_print(mp_obj_t o, mp_print_kind_t kind);
void mp_obj_print_exception(const mp_print_t *print, mp_obj_t exc);

bool mp_obj_is_true(mp_obj_t arg);
bool mp_obj_is_callable(mp_obj_t o_in);
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2);

mp_int_t mp_obj_get_int(mp_const_obj_t arg);
mp_int_t mp_obj_get_int_truncated(mp_const_obj_t arg);
bool mp_obj_get_int_maybe(mp_const_obj_t arg, mp_int_t *value);
#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mp_obj_get_float(mp_obj_t self_in);
bool mp_obj_get_float_maybe(mp_obj_t arg, mp_float_t *value);
void mp_obj_get_complex(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
#endif
//qstr mp_obj_get_qstr(mp_obj_t arg);
void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items); // *items may point inside a GC block
void mp_obj_get_array_fixed_n(mp_obj_t o, size_t len, mp_obj_t **items); // *items may point inside a GC block
size_t mp_get_index(const mp_obj_type_t *type, size_t len, mp_obj_t index, bool is_slice);
mp_obj_t mp_obj_id(mp_obj_t o_in);
mp_obj_t mp_obj_len(mp_obj_t o_in);
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in); // may return MP_OBJ_NULL
mp_obj_t mp_obj_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t val);
mp_obj_t mp_generic_unary_op(mp_unary_op_t op, mp_obj_t o_in);

// cell
mp_obj_t mp_obj_cell_get(mp_obj_t self_in);
void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj);

// int
// For long int, returns value truncated to mp_int_t
mp_int_t mp_obj_int_get_truncated(mp_const_obj_t self_in);
// Will raise exception if value doesn't fit into mp_int_t
mp_int_t mp_obj_int_get_checked(mp_const_obj_t self_in);

// exception
#define mp_obj_is_native_exception_instance(o) (mp_obj_get_type(o)->make_new == mp_obj_exception_make_new)
bool mp_obj_is_exception_type(mp_obj_t self_in);
bool mp_obj_is_exception_instance(mp_obj_t self_in);
bool mp_obj_exception_match(mp_obj_t exc, mp_const_obj_t exc_type);
void mp_obj_exception_clear_traceback(mp_obj_t self_in);
void mp_obj_exception_add_traceback(mp_obj_t self_in, qstr file, size_t line, qstr block);
void mp_obj_exception_get_traceback(mp_obj_t self_in, size_t *n, size_t **values);
mp_obj_t mp_obj_exception_get_value(mp_obj_t self_in);
mp_obj_t mp_obj_exception_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_alloc_emergency_exception_buf(mp_obj_t size_in);
void mp_init_emergency_exception_buf(void);

// str
bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2);
qstr mp_obj_str_get_qstr(mp_obj_t self_in); // use this if you will anyway convert the string to a qstr
const char *mp_obj_str_get_str(mp_obj_t self_in); // use this only if you need the string to be null terminated
const char *mp_obj_str_get_data(mp_obj_t self_in, size_t *len);
mp_obj_t mp_obj_str_intern(mp_obj_t str);
void mp_str_print_quoted(const mp_print_t *print, const byte *str_data, size_t str_len, bool is_bytes);

#if MICROPY_PY_BUILTINS_FLOAT
// float
#if MICROPY_FLOAT_HIGH_QUALITY_HASH
mp_int_t mp_float_hash(mp_float_t val);
#else
static inline mp_int_t mp_float_hash(mp_float_t val) { return (mp_int_t)val; }
#endif
mp_obj_t mp_obj_float_binary_op(mp_binary_op_t op, mp_float_t lhs_val, mp_obj_t rhs); // can return MP_OBJ_NULL if op not supported

// complex
void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
mp_obj_t mp_obj_complex_binary_op(mp_binary_op_t op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in); // can return MP_OBJ_NULL if op not supported
#else
#define mp_obj_is_float(o) (false)
#endif

// tuple
void mp_obj_tuple_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);
void mp_obj_tuple_del(mp_obj_t self_in);
mp_int_t mp_obj_tuple_hash(mp_obj_t self_in);

// list
struct _mp_obj_list_t;
void mp_obj_list_init(struct _mp_obj_list_t *o, size_t n);
mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
mp_obj_t mp_obj_list_remove(mp_obj_t self_in, mp_obj_t value);
void mp_obj_list_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);
void mp_obj_list_set_len(mp_obj_t self_in, size_t len);
void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_list_sort(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);

// dict
typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;
void mp_obj_dict_init(mp_obj_dict_t *dict, size_t n_args);
size_t mp_obj_dict_len(mp_obj_t self_in);
mp_obj_t mp_obj_dict_get(mp_obj_t self_in, mp_obj_t index);
mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
mp_obj_t mp_obj_dict_delete(mp_obj_t self_in, mp_obj_t key);
mp_map_t *mp_obj_dict_get_map(mp_obj_t self_in);

// set
void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item);

// slice
void mp_obj_slice_get(mp_obj_t self_in, mp_obj_t *start, mp_obj_t *stop, mp_obj_t *step);

// functions

typedef struct _mp_obj_fun_builtin_fixed_t {
    mp_obj_base_t base;
    union {
        mp_fun_0_t _0;
        mp_fun_1_t _1;
        mp_fun_2_t _2;
        mp_fun_3_t _3;
    } fun;
} mp_obj_fun_builtin_fixed_t;

#define MP_OBJ_FUN_ARGS_MAX (0xffff) // to set maximum value in n_args_max below
typedef struct _mp_obj_fun_builtin_var_t {
    mp_obj_base_t base;
    bool is_kw : 1;
    mp_uint_t n_args_min : 15; // inclusive
    mp_uint_t n_args_max : 16; // inclusive
    union {
        mp_fun_var_t var;
        mp_fun_kw_t kw;
    } fun;
} mp_obj_fun_builtin_var_t;

qstr mp_obj_fun_get_name(mp_const_obj_t fun);
qstr mp_obj_code_get_name(const byte *code_info);

mp_obj_t mp_identity(mp_obj_t self);
MP_DECLARE_CONST_FUN_OBJ_1(mp_identity_obj);
mp_obj_t mp_identity_getiter(mp_obj_t self, mp_obj_iter_buf_t *iter_buf);

// module
typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;
} mp_obj_module_t;
mp_obj_dict_t *mp_obj_module_get_globals(mp_obj_t self_in);
// check if given module object is a package
bool mp_obj_is_package(mp_obj_t module);

// staticmethod and classmethod types; defined here so we can make const versions
// this structure is used for instances of both staticmethod and classmethod
typedef struct _mp_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_obj_t fun;
} mp_obj_static_class_method_t;
typedef struct _mp_rom_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_rom_obj_t fun;
} mp_rom_obj_static_class_method_t;

// property
const mp_obj_t *mp_obj_property_get(mp_obj_t self_in);

// sequence helpers

// slice indexes resolved to particular sequence
typedef struct {
    mp_uint_t start;
    mp_uint_t stop;
    mp_int_t step;
} mp_bound_slice_t;

void mp_seq_multiply(const void *items, size_t item_sz, size_t len, size_t times, void *dest);
#if MICROPY_PY_BUILTINS_SLICE
bool mp_seq_get_fast_slice_indexes(mp_uint_t len, mp_obj_t slice, mp_bound_slice_t *indexes);
#endif
#define mp_seq_copy(dest, src, len, item_t) memcpy(dest, src, len * sizeof(item_t))
#define mp_seq_cat(dest, src1, len1, src2, len2, item_t) { memcpy(dest, src1, (len1) * sizeof(item_t)); memcpy(dest + (len1), src2, (len2) * sizeof(item_t)); }
bool mp_seq_cmp_bytes(mp_uint_t op, const byte *data1, size_t len1, const byte *data2, size_t len2);
bool mp_seq_cmp_objs(mp_uint_t op, const mp_obj_t *items1, size_t len1, const mp_obj_t *items2, size_t len2);
mp_obj_t mp_seq_index_obj(const mp_obj_t *items, size_t len, size_t n_args, const mp_obj_t *args);
mp_obj_t mp_seq_count_obj(const mp_obj_t *items, size_t len, mp_obj_t value);
mp_obj_t mp_seq_extract_slice(size_t len, const mp_obj_t *seq, mp_bound_slice_t *indexes);
// Helper to clear stale pointers from allocated, but unused memory, to preclude GC problems
#define mp_seq_clear(start, len, alloc_len, item_sz) memset((byte*)(start) + (len) * (item_sz), 0, ((alloc_len) - (len)) * (item_sz))
#define mp_seq_replace_slice_no_grow(dest, dest_len, beg, end, slice, slice_len, item_sz) \
    /*printf("memcpy(%p, %p, %d)\n", dest + beg, slice, slice_len * (item_sz));*/ \
    memcpy(((char*)dest) + (beg) * (item_sz), slice, slice_len * (item_sz)); \
    /*printf("memmove(%p, %p, %d)\n", dest + (beg + slice_len), dest + end, (dest_len - end) * (item_sz));*/ \
    memmove(((char*)dest) + (beg + slice_len) * (item_sz), ((char*)dest) + (end) * (item_sz), (dest_len - end) * (item_sz));

// Note: dest and slice regions may overlap
#define mp_seq_replace_slice_grow_inplace(dest, dest_len, beg, end, slice, slice_len, len_adj, item_sz) \
    /*printf("memmove(%p, %p, %d)\n", dest + beg + len_adj, dest + beg, (dest_len - beg) * (item_sz));*/ \
    memmove(((char*)dest) + (beg + slice_len) * (item_sz), ((char*)dest) + (end) * (item_sz), ((dest_len) + (len_adj) - ((beg) + (slice_len))) * (item_sz)); \
    memmove(((char*)dest) + (beg) * (item_sz), slice, slice_len * (item_sz));

#endif // MICROPY_INCLUDED_PY_OBJ_H
