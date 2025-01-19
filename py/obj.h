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

#include <assert.h>

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

#if MICROPY_DEBUG_MP_OBJ_SENTINELS
#define MP_OBJ_NULL             (MP_OBJ_FROM_PTR((void *)0))
#define MP_OBJ_STOP_ITERATION   (MP_OBJ_FROM_PTR((void *)4))
#define MP_OBJ_SENTINEL         (MP_OBJ_FROM_PTR((void *)8))
#else
#define MP_OBJ_NULL             (MP_OBJ_FROM_PTR((void *)0))
#define MP_OBJ_STOP_ITERATION   (MP_OBJ_FROM_PTR((void *)0))
#define MP_OBJ_SENTINEL         (MP_OBJ_FROM_PTR((void *)4))
#endif

// These macros/inline functions operate on objects and depend on the
// particular object representation.  They are used to query, pack and
// unpack small ints, qstrs and full object pointers.

#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A

static inline bool mp_obj_is_small_int(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 1) != 0;
}
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 1) | 1))

static inline bool mp_obj_is_qstr(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 2;
}
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 3)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 3) | 2))

static inline bool mp_obj_is_immediate_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 6;
}
#define MP_OBJ_IMMEDIATE_OBJ_VALUE(o) (((mp_uint_t)(o)) >> 3)
#define MP_OBJ_NEW_IMMEDIATE_OBJ(val) ((mp_obj_t)(((val) << 3) | 6))

#if MICROPY_PY_BUILTINS_FLOAT
#define mp_const_float_e MP_ROM_PTR(&mp_const_float_e_obj)
#define mp_const_float_pi MP_ROM_PTR(&mp_const_float_pi_obj)
#if MICROPY_PY_MATH_CONSTANTS
#define mp_const_float_tau MP_ROM_PTR(&mp_const_float_tau_obj)
#define mp_const_float_inf MP_ROM_PTR(&mp_const_float_inf_obj)
#define mp_const_float_nan MP_ROM_PTR(&mp_const_float_nan_obj)
#endif
extern const struct _mp_obj_float_t mp_const_float_e_obj;
extern const struct _mp_obj_float_t mp_const_float_pi_obj;
#if MICROPY_PY_MATH_CONSTANTS
extern const struct _mp_obj_float_t mp_const_float_tau_obj;
extern const struct _mp_obj_float_t mp_const_float_inf_obj;
extern const struct _mp_obj_float_t mp_const_float_nan_obj;
#endif

#define mp_obj_is_float(o) mp_obj_is_type((o), &mp_type_float)
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_new_float(mp_float_t value);
#endif

static inline bool mp_obj_is_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 3) == 0;
}

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B

static inline bool mp_obj_is_small_int(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 3) == 1;
}
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 2)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 2) | 1))

static inline bool mp_obj_is_qstr(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 3;
}
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 3)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 3) | 3))

static inline bool mp_obj_is_immediate_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 7;
}
#define MP_OBJ_IMMEDIATE_OBJ_VALUE(o) (((mp_uint_t)(o)) >> 3)
#define MP_OBJ_NEW_IMMEDIATE_OBJ(val) ((mp_obj_t)(((val) << 3) | 7))

#if MICROPY_PY_BUILTINS_FLOAT
#define mp_const_float_e MP_ROM_PTR(&mp_const_float_e_obj)
#define mp_const_float_pi MP_ROM_PTR(&mp_const_float_pi_obj)
#if MICROPY_PY_MATH_CONSTANTS
#define mp_const_float_tau MP_ROM_PTR(&mp_const_float_tau_obj)
#define mp_const_float_inf MP_ROM_PTR(&mp_const_float_inf_obj)
#define mp_const_float_nan MP_ROM_PTR(&mp_const_float_nan_obj)
#endif
extern const struct _mp_obj_float_t mp_const_float_e_obj;
extern const struct _mp_obj_float_t mp_const_float_pi_obj;
#if MICROPY_PY_MATH_CONSTANTS
extern const struct _mp_obj_float_t mp_const_float_tau_obj;
extern const struct _mp_obj_float_t mp_const_float_inf_obj;
extern const struct _mp_obj_float_t mp_const_float_nan_obj;
#endif

#define mp_obj_is_float(o) mp_obj_is_type((o), &mp_type_float)
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_new_float(mp_float_t value);
#endif

static inline bool mp_obj_is_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 1) == 0;
}

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_NONE
#error "MICROPY_OBJ_REPR_C requires float to be enabled."
#endif

static inline bool mp_obj_is_small_int(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 1) != 0;
}
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)((((mp_uint_t)(small_int)) << 1) | 1))

#if MICROPY_PY_BUILTINS_FLOAT
#define MP_OBJ_NEW_CONST_FLOAT(f) MP_ROM_PTR((mp_obj_t)((((((uint64_t)f) & ~3) | 2) + 0x80800000) & 0xffffffff))
#define mp_const_float_e  MP_OBJ_NEW_CONST_FLOAT(0x402df854)
#define mp_const_float_pi MP_OBJ_NEW_CONST_FLOAT(0x40490fdb)
#if MICROPY_PY_MATH_CONSTANTS
#define mp_const_float_tau MP_OBJ_NEW_CONST_FLOAT(0x40c90fdb)
#define mp_const_float_inf MP_OBJ_NEW_CONST_FLOAT(0x7f800000)
#define mp_const_float_nan MP_OBJ_NEW_CONST_FLOAT(0xffc00000)
#endif

static inline bool mp_obj_is_float(mp_const_obj_t o) {
    // Ensure that 32-bit arch can only use single precision.
    MP_STATIC_ASSERT(sizeof(mp_float_t) <= sizeof(mp_obj_t));

    return (((mp_uint_t)(o)) & 3) == 2 && (((mp_uint_t)(o)) & 0xff800007) != 0x00000006;
}
static inline mp_float_t mp_obj_float_get(mp_const_obj_t o) {
    union {
        mp_float_t f;
        mp_uint_t u;
    } num = {.u = ((mp_uint_t)o - 0x80800000u) & ~3u};
    return num.f;
}
static inline mp_obj_t mp_obj_new_float(mp_float_t f) {
    union {
        mp_float_t f;
        mp_uint_t u;
    } num = {.f = f};
    return (mp_obj_t)(((num.u & ~0x3u) | 2u) + 0x80800000u);
}
#endif

static inline bool mp_obj_is_qstr(mp_const_obj_t o) {
    return (((mp_uint_t)(o)) & 0xff80000f) == 0x00000006;
}
#define MP_OBJ_QSTR_VALUE(o) (((mp_uint_t)(o)) >> 4)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)((((mp_uint_t)(qst)) << 4) | 0x00000006))

static inline bool mp_obj_is_immediate_obj(mp_const_obj_t o) {
    return (((mp_uint_t)(o)) & 0xff80000f) == 0x0000000e;
}
#define MP_OBJ_IMMEDIATE_OBJ_VALUE(o) (((mp_uint_t)(o)) >> 4)
#define MP_OBJ_NEW_IMMEDIATE_OBJ(val) ((mp_obj_t)(((val) << 4) | 0xe))

static inline bool mp_obj_is_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 3) == 0;
}

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D

static inline bool mp_obj_is_small_int(mp_const_obj_t o) {
    return (((uint64_t)(o)) & 0xffff000000000000) == 0x0001000000000000;
}
#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_int_t)((o) << 16)) >> 17)
#define MP_OBJ_NEW_SMALL_INT(small_int) (((((uint64_t)(small_int)) & 0x7fffffffffff) << 1) | 0x0001000000000001)

static inline bool mp_obj_is_qstr(mp_const_obj_t o) {
    return (((uint64_t)(o)) & 0xffff000000000000) == 0x0002000000000000;
}
#define MP_OBJ_QSTR_VALUE(o) ((((uint32_t)(o)) >> 1) & 0xffffffff)
#define MP_OBJ_NEW_QSTR(qst) ((mp_obj_t)(((uint64_t)(((uint32_t)(qst)) << 1)) | 0x0002000000000001))

static inline bool mp_obj_is_immediate_obj(mp_const_obj_t o) {
    return (((uint64_t)(o)) & 0xffff000000000000) == 0x0003000000000000;
}
#define MP_OBJ_IMMEDIATE_OBJ_VALUE(o) ((((uint32_t)(o)) >> 46) & 3)
#define MP_OBJ_NEW_IMMEDIATE_OBJ(val) (((uint64_t)(val) << 46) | 0x0003000000000000)

#if MICROPY_PY_BUILTINS_FLOAT

#if MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_DOUBLE
#error MICROPY_OBJ_REPR_D requires MICROPY_FLOAT_IMPL_DOUBLE
#endif

#define mp_const_float_e {((mp_obj_t)((uint64_t)0x4005bf0a8b145769 + 0x8004000000000000))}
#define mp_const_float_pi {((mp_obj_t)((uint64_t)0x400921fb54442d18 + 0x8004000000000000))}
#if MICROPY_PY_MATH_CONSTANTS
#define mp_const_float_tau {((mp_obj_t)((uint64_t)0x401921fb54442d18 + 0x8004000000000000))}
#define mp_const_float_inf {((mp_obj_t)((uint64_t)0x7ff0000000000000 + 0x8004000000000000))}
#define mp_const_float_nan {((mp_obj_t)((uint64_t)0xfff8000000000000 + 0x8004000000000000))}
#endif

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

static inline bool mp_obj_is_obj(mp_const_obj_t o) {
    return (((uint64_t)(o)) & 0xffff000000000000) == 0x0000000000000000;
}
#define MP_OBJ_TO_PTR(o) ((void *)(uintptr_t)(o))
#define MP_OBJ_FROM_PTR(p) ((mp_obj_t)((uintptr_t)(p)))

// rom object storage needs special handling to widen 32-bit pointer to 64-bits
typedef union _mp_rom_obj_t {
    uint64_t u64;
    struct {
        const void *lo, *hi;
    } u32;
} mp_rom_obj_t;
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
#define MP_OBJ_TO_PTR(o) ((void *)(o))
#endif

// Cast object pointer to mp_obj_t
#ifndef MP_OBJ_FROM_PTR
#define MP_OBJ_FROM_PTR(p) ((mp_obj_t)(p))
#endif

// Macros to create objects that are stored in ROM.

#ifndef MP_ROM_NONE
#if MICROPY_OBJ_IMMEDIATE_OBJS
#define MP_ROM_NONE mp_const_none
#else
#define MP_ROM_NONE MP_ROM_PTR(&mp_const_none_obj)
#endif
#endif

#ifndef MP_ROM_FALSE
#if MICROPY_OBJ_IMMEDIATE_OBJS
#define MP_ROM_FALSE mp_const_false
#define MP_ROM_TRUE mp_const_true
#else
#define MP_ROM_FALSE MP_ROM_PTR(&mp_const_false_obj)
#define MP_ROM_TRUE MP_ROM_PTR(&mp_const_true_obj)
#endif
#endif

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

// These macros are used to declare and define constant function objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_FUN_OBJ_0(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_1(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_2(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_3(obj_name) extern const mp_obj_fun_builtin_fixed_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_VAR(obj_name) extern const mp_obj_fun_builtin_var_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name) extern const mp_obj_fun_builtin_var_t obj_name
#define MP_DECLARE_CONST_FUN_OBJ_KW(obj_name) extern const mp_obj_fun_builtin_var_t obj_name

#define MP_OBJ_FUN_ARGS_MAX (0xffff) // to set maximum value in n_args_max below
#define MP_OBJ_FUN_MAKE_SIG(n_args_min, n_args_max, takes_kw) ((uint32_t)((((uint32_t)(n_args_min)) << 17) | (((uint32_t)(n_args_max)) << 1) | ((takes_kw) ? 1 : 0)))

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
    {{&mp_type_fun_builtin_var}, MP_OBJ_FUN_MAKE_SIG(n_args_min, MP_OBJ_FUN_ARGS_MAX, false), .fun.var = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name, n_args_min, n_args_max, fun_name) \
    const mp_obj_fun_builtin_var_t obj_name = \
    {{&mp_type_fun_builtin_var}, MP_OBJ_FUN_MAKE_SIG(n_args_min, n_args_max, false), .fun.var = fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_KW(obj_name, n_args_min, fun_name) \
    const mp_obj_fun_builtin_var_t obj_name = \
    {{&mp_type_fun_builtin_var}, MP_OBJ_FUN_MAKE_SIG(n_args_min, MP_OBJ_FUN_ARGS_MAX, true), .fun.kw = fun_name}

// These macros are used to define constant map/dict objects
// You can put "static" in front of the definition to make it local

#define MP_DEFINE_CONST_MAP(map_name, table_name) \
    const mp_map_t map_name = { \
        .all_keys_are_qstrs = 1, \
        .is_fixed = 1, \
        .is_ordered = 1, \
        .used = MP_ARRAY_SIZE(table_name), \
        .alloc = MP_ARRAY_SIZE(table_name), \
        .table = (mp_map_elem_t *)(mp_rom_map_elem_t *)table_name, \
    }

#define MP_DEFINE_CONST_DICT_WITH_SIZE(dict_name, table_name, n) \
    const mp_obj_dict_t dict_name = { \
        .base = {&mp_type_dict}, \
        .map = { \
            .all_keys_are_qstrs = 1, \
            .is_fixed = 1, \
            .is_ordered = 1, \
            .used = n, \
            .alloc = n, \
            .table = (mp_map_elem_t *)(mp_rom_map_elem_t *)table_name, \
        }, \
    }

#define MP_DEFINE_CONST_DICT(dict_name, table_name) MP_DEFINE_CONST_DICT_WITH_SIZE(dict_name, table_name, MP_ARRAY_SIZE(table_name))

// These macros are used to declare and define constant staticmethod and classmethod objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_STATICMETHOD_OBJ(obj_name) extern const mp_rom_obj_static_class_method_t obj_name
#define MP_DECLARE_CONST_CLASSMETHOD_OBJ(obj_name) extern const mp_rom_obj_static_class_method_t obj_name

#define MP_DEFINE_CONST_STATICMETHOD_OBJ(obj_name, fun_name) const mp_rom_obj_static_class_method_t obj_name = {{&mp_type_staticmethod}, fun_name}
#define MP_DEFINE_CONST_CLASSMETHOD_OBJ(obj_name, fun_name) const mp_rom_obj_static_class_method_t obj_name = {{&mp_type_classmethod}, fun_name}

#ifndef NO_QSTR

// Declare a module as a builtin, processed by makemoduledefs.py
// param module_name: MP_QSTR_<module name>
// param obj_module: mp_obj_module_t instance
#define MP_REGISTER_MODULE(module_name, obj_module)

// As above, but allow this module to be extended from the filesystem.
#define MP_REGISTER_EXTENSIBLE_MODULE(module_name, obj_module)

// Add a custom handler for a builtin module that will be called to delegate
// failed attribute lookups.
#define MP_REGISTER_MODULE_DELEGATION(obj_module, fun_name)

// Declare a root pointer (to avoid garbage collection of a global static variable).
// param variable_declaration: a valid C variable declaration
#define MP_REGISTER_ROOT_POINTER(variable_declaration)

#endif // NO_QSTR

// Underlying map/hash table implementation (not dict object or map function)

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct _mp_rom_map_elem_t {
    mp_rom_obj_t key;
    mp_rom_obj_t value;
} mp_rom_map_elem_t;

typedef struct _mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;    // if set, table is fixed/read-only and can't be modified
    size_t is_ordered : 1;  // if set, table is an ordered array, not a hash map
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

static inline bool mp_map_slot_is_filled(const mp_map_t *map, size_t pos) {
    assert(pos < map->alloc);
    return (map)->table[pos].key != MP_OBJ_NULL && (map)->table[pos].key != MP_OBJ_SENTINEL;
}

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

static inline bool mp_set_slot_is_filled(const mp_set_t *set, size_t pos) {
    return (set)->table[pos] != MP_OBJ_NULL && (set)->table[pos] != MP_OBJ_SENTINEL;
}

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
// Note that the mp_obj_t* array will contain all arguments, positional and keyword, with the keyword
// ones starting at offset n, like: arg0 arg1 ... arg<n> key0 value0 key1 value1 ..., and the mp_map_t*
// gets those same keyword arguments but as a map for convenience; see fun_builtin_var_call.
typedef mp_obj_t (*mp_fun_kw_t)(size_t n, const mp_obj_t *, mp_map_t *);

// Flags for type behaviour (mp_obj_type_t.flags)
// If MP_TYPE_FLAG_EQ_NOT_REFLEXIVE is clear then __eq__ is reflexive (A==A returns True).
// If MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE is clear then the type can't be equal to an
//   instance of any different class that also clears this flag.  If this flag is set
//   then the type may check for equality against a different type.
// If MP_TYPE_FLAG_EQ_HAS_NEQ_TEST is clear then the type only implements the __eq__
//   operator and not the __ne__ operator.  If it's set then __ne__ may be implemented.
// If MP_TYPE_FLAG_BINDS_SELF is set then the type as a method binds self as the first arg.
// If MP_TYPE_FLAG_BUILTIN_FUN is set then the type is a built-in function type.
// MP_TYPE_FLAG_ITER_IS_GETITER is a no-op flag that means the default behaviour for the
//   iter slot and it's the getiter function.
// If MP_TYPE_FLAG_ITER_IS_ITERNEXT is set then the "iter" slot is the iternext
//   function and getiter will be automatically implemented as "return self".
// If MP_TYPE_FLAG_ITER_IS_CUSTOM is set then the "iter" slot is a pointer to a
//   mp_getiter_iternext_custom_t struct instance (with both .getiter and .iternext set).
// If MP_TYPE_FLAG_ITER_IS_STREAM is set then the type implicitly gets a "return self"
//   getiter, and mp_stream_unbuffered_iter for iternext.
// If MP_TYPE_FLAG_INSTANCE_TYPE is set then this is an instance type (i.e. defined in Python).
#define MP_TYPE_FLAG_NONE (0x0000)
#define MP_TYPE_FLAG_IS_SUBCLASSED (0x0001)
#define MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS (0x0002)
#define MP_TYPE_FLAG_EQ_NOT_REFLEXIVE (0x0004)
#define MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE (0x0008)
#define MP_TYPE_FLAG_EQ_HAS_NEQ_TEST (0x0010)
#define MP_TYPE_FLAG_BINDS_SELF (0x0020)
#define MP_TYPE_FLAG_BUILTIN_FUN (0x0040)
#define MP_TYPE_FLAG_ITER_IS_GETITER (0x0000)
#define MP_TYPE_FLAG_ITER_IS_ITERNEXT (0x0080)
#define MP_TYPE_FLAG_ITER_IS_CUSTOM (0x0100)
#define MP_TYPE_FLAG_ITER_IS_STREAM (MP_TYPE_FLAG_ITER_IS_ITERNEXT | MP_TYPE_FLAG_ITER_IS_CUSTOM)
#define MP_TYPE_FLAG_INSTANCE_TYPE (0x0200)

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
typedef mp_fun_1_t mp_iternext_fun_t;

// For MP_TYPE_FLAG_ITER_IS_CUSTOM, the "getiter" slot points to an instance of this type.
typedef struct _mp_getiter_iternext_custom_t {
    mp_getiter_fun_t getiter;
    mp_iternext_fun_t iternext;
} mp_getiter_iternext_custom_t;

// Buffer protocol

typedef struct _mp_buffer_info_t {
    void *buf;      // can be NULL if len == 0
    size_t len;     // in bytes
    int typecode;   // as per binary.h
} mp_buffer_info_t;

#define MP_BUFFER_READ  (1)
#define MP_BUFFER_WRITE (2)
#define MP_BUFFER_RW (MP_BUFFER_READ | MP_BUFFER_WRITE)
#define MP_BUFFER_RAISE_IF_UNSUPPORTED (4)

typedef mp_int_t (*mp_buffer_fun_t)(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);

bool mp_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);

static inline void mp_get_buffer_raise(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_get_buffer(obj, bufinfo, flags | MP_BUFFER_RAISE_IF_UNSUPPORTED);
}

// This struct will be updated to become a variable sized struct. In order to
// use this as a member, or allocate dynamically, use the mp_obj_empty_type_t
// or mp_obj_full_type_t structs below (which must be kept in sync).
struct _mp_obj_type_t {
    // A type is an object so must start with this entry, which points to mp_type_type.
    mp_obj_base_t base;

    // Flags associated with this type.
    uint16_t flags;

    // The name of this type, a qstr.
    uint16_t name;

    // Slots: For the rest of the fields, the slot index points to the
    // relevant function in the variable-length "slots" field. Ideally these
    // would be only 4 bits, but the extra overhead of accessing them adds
    // more code, and we also need to be able to take the address of them for
    // mp_obj_class_lookup.

    // Corresponds to __new__ and __init__ special methods, to make an instance of the type.
    uint8_t slot_index_make_new;

    // Corresponds to __repr__ and __str__ special methods.
    uint8_t slot_index_print;

    // Corresponds to __call__ special method, ie T(...).
    uint8_t slot_index_call;

    // Implements unary and binary operations.
    // Can return MP_OBJ_NULL if the operation is not supported.
    uint8_t slot_index_unary_op;
    uint8_t slot_index_binary_op;

    // Implements load, store and delete attribute.
    //
    // dest[0] = MP_OBJ_NULL means load
    //  return: for fail, do nothing
    //          for fail but continue lookup in locals_dict, dest[1] = MP_OBJ_SENTINEL
    //          for attr, dest[0] = value
    //          for method, dest[0] = method, dest[1] = self
    //
    // dest[0,1] = {MP_OBJ_SENTINEL, MP_OBJ_NULL} means delete
    // dest[0,1] = {MP_OBJ_SENTINEL, object} means store
    //  return: for fail, do nothing
    //          for success set dest[0] = MP_OBJ_NULL
    uint8_t slot_index_attr;

    // Implements load, store and delete subscripting:
    //  - value = MP_OBJ_SENTINEL means load
    //  - value = MP_OBJ_NULL means delete
    //  - all other values mean store the value
    // Can return MP_OBJ_NULL if operation not supported.
    uint8_t slot_index_subscr;

    // This slot's behaviour depends on the MP_TYPE_FLAG_ITER_IS_* flags above.
    // - If MP_TYPE_FLAG_ITER_IS_GETITER flag is set, then this corresponds to the __iter__
    //   special method (of type mp_getiter_fun_t). Can use the given mp_obj_iter_buf_t
    //   to store the iterator object, otherwise can return a pointer to an object on the heap.
    // - If MP_TYPE_FLAG_ITER_IS_ITERNEXT is set, then this corresponds to __next__ special method.
    //   May return MP_OBJ_STOP_ITERATION as an optimisation instead of raising StopIteration()
    //   with no args. The type will implicitly implement getiter as "return self".
    // - If MP_TYPE_FLAG_ITER_IS_CUSTOM is set, then this slot must point to an
    //   mp_getiter_iternext_custom_t instance with both the getiter and iternext fields set.
    // - If MP_TYPE_FLAG_ITER_IS_STREAM is set, this this slot should be unset.
    uint8_t slot_index_iter;

    // Implements the buffer protocol if supported by this type.
    uint8_t slot_index_buffer;

    // One of disjoint protocols (interfaces), like mp_stream_p_t, etc.
    uint8_t slot_index_protocol;

    // A pointer to the parents of this type:
    //  - 0 parents: pointer is NULL (object is implicitly the single parent)
    //  - 1 parent: a pointer to the type of that parent
    //  - 2 or more parents: pointer to a tuple object containing the parent types
    uint8_t slot_index_parent;

    // A dict mapping qstrs to objects local methods/constants/etc.
    uint8_t slot_index_locals_dict;

    const void *slots[];
};

// Non-variable sized versions of mp_obj_type_t to be used as a member
// in other structs or for dynamic allocation. The fields are exactly
// as in mp_obj_type_t, but with a fixed size for the flexible array
// members.
typedef struct _mp_obj_empty_type_t {
    mp_obj_base_t base;
    uint16_t flags;
    uint16_t name;

    uint8_t slot_index_make_new;
    uint8_t slot_index_print;
    uint8_t slot_index_call;
    uint8_t slot_index_unary_op;
    uint8_t slot_index_binary_op;
    uint8_t slot_index_attr;
    uint8_t slot_index_subscr;
    uint8_t slot_index_iter;
    uint8_t slot_index_buffer;
    uint8_t slot_index_protocol;
    uint8_t slot_index_parent;
    uint8_t slot_index_locals_dict;

    // No slots member.
} mp_obj_empty_type_t;

typedef struct _mp_obj_full_type_t {
    mp_obj_base_t base;
    uint16_t flags;
    uint16_t name;

    uint8_t slot_index_make_new;
    uint8_t slot_index_print;
    uint8_t slot_index_call;
    uint8_t slot_index_unary_op;
    uint8_t slot_index_binary_op;
    uint8_t slot_index_attr;
    uint8_t slot_index_subscr;
    uint8_t slot_index_iter;
    uint8_t slot_index_buffer;
    uint8_t slot_index_protocol;
    uint8_t slot_index_parent;
    uint8_t slot_index_locals_dict;

    // Explicitly add 12 slots.
    const void *slots[11];
} mp_obj_full_type_t;

#define _MP_OBJ_TYPE_SLOT_TYPE_make_new (mp_make_new_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_print (mp_print_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_call (mp_call_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_unary_op (mp_unary_op_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_binary_op (mp_binary_op_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_attr (mp_attr_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_subscr (mp_subscr_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_iter (const void *)
#define _MP_OBJ_TYPE_SLOT_TYPE_buffer (mp_buffer_fun_t)
#define _MP_OBJ_TYPE_SLOT_TYPE_protocol (const void *)
#define _MP_OBJ_TYPE_SLOT_TYPE_parent (const void *)
#define _MP_OBJ_TYPE_SLOT_TYPE_locals_dict (struct _mp_obj_dict_t *)

// Implementation of MP_DEFINE_CONST_OBJ_TYPE for each number of arguments.
// Do not use these directly, instead use MP_DEFINE_CONST_OBJ_TYPE.
// Generated with:
// for i in range(13):
//     print(f"#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_{i}(_struct_type, _typename, _name, _flags{''.join(f', f{j+1}, v{j+1}' for j in range(i))}) const _struct_type _typename = {{ .base = {{ &mp_type_type }}, .flags = _flags, .name = _name{''.join(f', .slot_index_##f{j+1} = {j+1}' for j in range(i))}{', .slots = { ' + ''.join(f'v{j+1}, ' for j in range(i)) + '}' if i else '' } }}")
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_0(_struct_type, _typename, _name, _flags) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_1(_struct_type, _typename, _name, _flags, f1, v1) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slots = { v1, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_2(_struct_type, _typename, _name, _flags, f1, v1, f2, v2) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slots = { v1, v2, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_3(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slots = { v1, v2, v3, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_4(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slots = { v1, v2, v3, v4, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_5(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slots = { v1, v2, v3, v4, v5, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_6(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slots = { v1, v2, v3, v4, v5, v6, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_7(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slots = { v1, v2, v3, v4, v5, v6, v7, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_8(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_9(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_10(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_11(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10, f11, v11) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slot_index_##f11 = 11, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, } }
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS_12(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10, f11, v11, f12, v12) const _struct_type _typename = { .base = { &mp_type_type }, .flags = _flags, .name = _name, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slot_index_##f11 = 11, .slot_index_##f12 = 12, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, } }

// Because the mp_obj_type_t instances are in (zero-initialised) ROM, we take
// slot_index_foo=0 to mean that the slot is unset. This also simplifies checking
// if the slot is set. That means that we need to store index+1 in slot_index_foo
// though and then access it as slots[slot_index_foo - 1]. This is an implementation
// detail, the user of these macros doesn't need to be aware of it, and when using
// MP_OBJ_TYPE_OFFSETOF_SLOT you should use zero-based indexing.
#define MP_OBJ_TYPE_HAS_SLOT(t, f) ((t)->slot_index_##f)
#define MP_OBJ_TYPE_GET_SLOT(t, f) (_MP_OBJ_TYPE_SLOT_TYPE_##f(t)->slots[(t)->slot_index_##f - 1])
#define MP_OBJ_TYPE_GET_SLOT_OR_NULL(t, f) (_MP_OBJ_TYPE_SLOT_TYPE_##f(MP_OBJ_TYPE_HAS_SLOT(t, f) ? MP_OBJ_TYPE_GET_SLOT(t, f) : NULL))
#define MP_OBJ_TYPE_SET_SLOT(t, f, v, n) ((t)->slot_index_##f = (n) + 1, (t)->slots[(n)] = (void *)v)
#define MP_OBJ_TYPE_OFFSETOF_SLOT(f) (offsetof(mp_obj_type_t, slot_index_##f))
#define MP_OBJ_TYPE_HAS_SLOT_BY_OFFSET(t, offset) (*(uint8_t *)((char *)(t) + (offset)) != 0)

// Workaround for https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=msvc-160#macro-arguments-are-unpacked
#define MP_DEFINE_CONST_OBJ_TYPE_EXPAND(x) x

// This macro evaluates to MP_DEFINE_CONST_OBJ_TYPE_NARGS_##N, where N is the value
// of the 29th argument (29 is 13*2 + 3).
#define MP_DEFINE_CONST_OBJ_TYPE_NARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, N, ...) MP_DEFINE_CONST_OBJ_TYPE_NARGS_##N

// This macros is used to define a object type in ROM.
// Invoke as MP_DEFINE_CONST_OBJ_TYPE(_typename, _name, _flags, _make_new [, slot, func]*)
// It uses the number of arguments to select which MP_DEFINE_CONST_OBJ_TYPE_*
// macro to use based on the number of arguments. It works by shifting the
// numeric values 12, 11, ... 0 by the number of arguments, such that the
// 29th argument ends up being the number to use. The _INV values are
// placeholders because the slot arguments come in pairs.
#define MP_DEFINE_CONST_OBJ_TYPE(...) MP_DEFINE_CONST_OBJ_TYPE_EXPAND(MP_DEFINE_CONST_OBJ_TYPE_NARGS(__VA_ARGS__, _INV, 12, _INV, 11, _INV, 10, _INV, 9, _INV, 8, _INV, 7, _INV, 6, _INV, 5, _INV, 4, _INV, 3, _INV, 2, _INV, 1, _INV, 0)(mp_obj_type_t, __VA_ARGS__))

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
extern const mp_obj_type_t mp_type_deque;
extern const mp_obj_type_t mp_type_dict;
extern const mp_obj_type_t mp_type_ordereddict;
extern const mp_obj_type_t mp_type_range;
extern const mp_obj_type_t mp_type_set;
extern const mp_obj_type_t mp_type_frozenset;
extern const mp_obj_type_t mp_type_slice;
extern const mp_obj_type_t mp_type_zip;
extern const mp_obj_type_t mp_type_array;
extern const mp_obj_type_t mp_type_super;
extern const mp_obj_type_t mp_type_gen_wrap;
extern const mp_obj_type_t mp_type_native_gen_wrap;
extern const mp_obj_type_t mp_type_gen_instance;
extern const mp_obj_type_t mp_type_fun_builtin_0;
extern const mp_obj_type_t mp_type_fun_builtin_1;
extern const mp_obj_type_t mp_type_fun_builtin_2;
extern const mp_obj_type_t mp_type_fun_builtin_3;
extern const mp_obj_type_t mp_type_fun_builtin_var;
extern const mp_obj_type_t mp_type_fun_bc;
extern const mp_obj_type_t mp_type_fun_native;
extern const mp_obj_type_t mp_type_fun_viper;
extern const mp_obj_type_t mp_type_fun_asm;
extern const mp_obj_type_t mp_type_code;
extern const mp_obj_type_t mp_type_module;
extern const mp_obj_type_t mp_type_staticmethod;
extern const mp_obj_type_t mp_type_classmethod;
extern const mp_obj_type_t mp_type_bound_meth;
extern const mp_obj_type_t mp_type_property;
extern const mp_obj_type_t mp_type_stringio;
extern const mp_obj_type_t mp_type_bytesio;
extern const mp_obj_type_t mp_type_ringio;
extern const mp_obj_type_t mp_type_reversed;
extern const mp_obj_type_t mp_type_polymorph_iter;
#if MICROPY_ENABLE_FINALISER
extern const mp_obj_type_t mp_type_polymorph_iter_with_finaliser;
#endif

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

// Constant objects, globally accessible: None, False, True
// These should always be accessed via the below macros.
#if MICROPY_OBJ_IMMEDIATE_OBJS
// None is even while False/True are odd so their types can be distinguished with 1 bit.
#define mp_const_none MP_OBJ_NEW_IMMEDIATE_OBJ(0)
#define mp_const_false MP_OBJ_NEW_IMMEDIATE_OBJ(1)
#define mp_const_true MP_OBJ_NEW_IMMEDIATE_OBJ(3)
#else
#define mp_const_none (MP_OBJ_FROM_PTR(&mp_const_none_obj))
#define mp_const_false (MP_OBJ_FROM_PTR(&mp_const_false_obj))
#define mp_const_true (MP_OBJ_FROM_PTR(&mp_const_true_obj))
extern const struct _mp_obj_none_t mp_const_none_obj;
extern const struct _mp_obj_bool_t mp_const_false_obj;
extern const struct _mp_obj_bool_t mp_const_true_obj;
#endif

// Constant objects, globally accessible: b'', (), {}, Ellipsis, NotImplemented, GeneratorExit()
// The below macros are for convenience only.
#define mp_const_empty_bytes (MP_OBJ_FROM_PTR(&mp_const_empty_bytes_obj))
#define mp_const_empty_tuple (MP_OBJ_FROM_PTR(&mp_const_empty_tuple_obj))
#define mp_const_notimplemented (MP_OBJ_FROM_PTR(&mp_const_notimplemented_obj))
extern const struct _mp_obj_str_t mp_const_empty_bytes_obj;
extern const struct _mp_obj_tuple_t mp_const_empty_tuple_obj;
extern const struct _mp_obj_dict_t mp_const_empty_dict_obj;
extern const struct _mp_obj_singleton_t mp_const_ellipsis_obj;
extern const struct _mp_obj_singleton_t mp_const_notimplemented_obj;
extern const struct _mp_obj_exception_t mp_const_GeneratorExit_obj;

// Fixed empty map. Useful when calling keyword-receiving functions
// without any keywords from C, etc.
#define mp_const_empty_map (mp_const_empty_dict_obj.map)

// General API for objects

// Helper versions of m_new_obj when you need to immediately set base.type.
// Implementing this as a call rather than inline saves 8 bytes per usage.
#define mp_obj_malloc(struct_type, obj_type) ((struct_type *)mp_obj_malloc_helper(sizeof(struct_type), obj_type))
#define mp_obj_malloc_var(struct_type, var_field, var_type, var_num, obj_type) ((struct_type *)mp_obj_malloc_helper(offsetof(struct_type, var_field) + sizeof(var_type) * (var_num), obj_type))
void *mp_obj_malloc_helper(size_t num_bytes, const mp_obj_type_t *type);

// Object allocation macros for allocating objects that have a finaliser.
#if MICROPY_ENABLE_FINALISER
#define mp_obj_malloc_with_finaliser(struct_type, obj_type) ((struct_type *)mp_obj_malloc_with_finaliser_helper(sizeof(struct_type), obj_type))
#define mp_obj_malloc_var_with_finaliser(struct_type, var_type, var_num, obj_type) ((struct_type *)mp_obj_malloc_with_finaliser_helper(sizeof(struct_type) + sizeof(var_type) * (var_num), obj_type))
void *mp_obj_malloc_with_finaliser_helper(size_t num_bytes, const mp_obj_type_t *type);
#else
#define mp_obj_malloc_with_finaliser(struct_type, obj_type) mp_obj_malloc(struct_type, obj_type)
#define mp_obj_malloc_var_with_finaliser(struct_type, var_type, var_num, obj_type) mp_obj_malloc_var(struct_type, var_type, var_num, obj_type)
#endif

// These macros are derived from more primitive ones and are used to
// check for more specific object types.
// Note: these are kept as macros because inline functions sometimes use much
// more code space than the equivalent macros, depending on the compiler.
// don't use mp_obj_is_exact_type directly; use mp_obj_is_type which provides additional safety checks.
// use the former only if you need to bypass these checks (because you've already checked everything else)
#define mp_obj_is_exact_type(o, t) (mp_obj_is_obj(o) && (((mp_obj_base_t *)MP_OBJ_TO_PTR(o))->type == (t)))

// Type checks are split to a separate, constant result macro. This is so it doesn't hinder the compilers's
// optimizations (other tricks like using ({ expr; exper; }) or (exp, expr, expr) in mp_obj_is_type() result
// in missed optimizations)
#define mp_type_assert_not_bool_int_str_nonetype(t) (                                     \
    MP_STATIC_ASSERT_NONCONSTEXPR((t) != &mp_type_bool), assert((t) != &mp_type_bool),         \
    MP_STATIC_ASSERT_NONCONSTEXPR((t) != &mp_type_int), assert((t) != &mp_type_int),           \
    MP_STATIC_ASSERT_NONCONSTEXPR((t) != &mp_type_str), assert((t) != &mp_type_str),           \
    MP_STATIC_ASSERT_NONCONSTEXPR((t) != &mp_type_NoneType), assert((t) != &mp_type_NoneType), \
    1)

#define mp_obj_is_type(o, t) (mp_type_assert_not_bool_int_str_nonetype(t) && mp_obj_is_exact_type(o, t))
#if MICROPY_OBJ_IMMEDIATE_OBJS
// bool's are immediates, not real objects, so test for the 2 possible values.
#define mp_obj_is_bool(o) ((o) == mp_const_false || (o) == mp_const_true)
#else
#define mp_obj_is_bool(o) mp_obj_is_exact_type(o, &mp_type_bool)
#endif
#define mp_obj_is_int(o) (mp_obj_is_small_int(o) || mp_obj_is_exact_type(o, &mp_type_int))
#define mp_obj_is_str(o) (mp_obj_is_qstr(o) || mp_obj_is_exact_type(o, &mp_type_str))
#define mp_obj_is_str_or_bytes(o) (mp_obj_is_qstr(o) || (mp_obj_is_obj(o) && MP_OBJ_TYPE_GET_SLOT_OR_NULL(((mp_obj_base_t *)MP_OBJ_TO_PTR(o))->type, binary_op) == mp_obj_str_binary_op))
bool mp_obj_is_dict_or_ordereddict(mp_obj_t o);
#define mp_obj_is_fun(o) (mp_obj_is_obj(o) && (((mp_obj_base_t *)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_function))

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict);
static inline mp_obj_t mp_obj_new_bool(mp_int_t x) {
    return x ? mp_const_true : mp_const_false;
}
mp_obj_t mp_obj_new_cell(mp_obj_t obj);
mp_obj_t mp_obj_new_int(mp_int_t value);
mp_obj_t mp_obj_new_int_from_uint(mp_uint_t value);
mp_obj_t mp_obj_new_int_from_str_len(const char **str, size_t len, bool neg, unsigned int base);
mp_obj_t mp_obj_new_int_from_ll(long long val); // this must return a multi-precision integer object (or raise an overflow exception)
mp_obj_t mp_obj_new_int_from_ull(unsigned long long val); // this must return a multi-precision integer object (or raise an overflow exception)
mp_obj_t mp_obj_new_str(const char *data, size_t len); // will check utf-8 (raises UnicodeError)
mp_obj_t mp_obj_new_str_from_cstr(const char *str); // // accepts null-terminated string, will check utf-8 (raises UnicodeError)
mp_obj_t mp_obj_new_str_via_qstr(const char *data, size_t len); // input data must be valid utf-8
mp_obj_t mp_obj_new_str_from_vstr(vstr_t *vstr); // will check utf-8 (raises UnicodeError)
#if MICROPY_PY_BUILTINS_STR_UNICODE && MICROPY_PY_BUILTINS_STR_UNICODE_CHECK
mp_obj_t mp_obj_new_str_from_utf8_vstr(vstr_t *vstr); // input data must be valid utf-8
#else
#define mp_obj_new_str_from_utf8_vstr mp_obj_new_str_from_vstr
#endif
mp_obj_t mp_obj_new_bytes_from_vstr(vstr_t *vstr);
mp_obj_t mp_obj_new_bytes(const byte *data, size_t len);
mp_obj_t mp_obj_new_bytearray(size_t n, const void *items);
mp_obj_t mp_obj_new_bytearray_by_ref(size_t n, void *items);
#if MICROPY_PY_BUILTINS_FLOAT
mp_obj_t mp_obj_new_int_from_float(mp_float_t val);
mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);
#endif
mp_obj_t mp_obj_new_exception(const mp_obj_type_t *exc_type);
mp_obj_t mp_obj_new_exception_args(const mp_obj_type_t *exc_type, size_t n_args, const mp_obj_t *args);
#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NONE
#define mp_obj_new_exception_msg(exc_type, msg) mp_obj_new_exception(exc_type)
#define mp_obj_new_exception_msg_varg(exc_type, ...) mp_obj_new_exception(exc_type)
#else
mp_obj_t mp_obj_new_exception_msg(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg);
mp_obj_t mp_obj_new_exception_msg_varg(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, ...); // counts args by number of % symbols in fmt, excluding %%; can only handle void* sizes (ie no float/double!)
#endif
#ifdef va_start
mp_obj_t mp_obj_new_exception_msg_vlist(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, va_list arg); // same fmt restrictions as above
#endif
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

const mp_obj_type_t *mp_obj_get_type(mp_const_obj_t o_in);
const char *mp_obj_get_type_str(mp_const_obj_t o_in);
bool mp_obj_is_subclass_fast(mp_const_obj_t object, mp_const_obj_t classinfo); // arguments should be type objects
mp_obj_t mp_obj_cast_to_native_base(mp_obj_t self_in, mp_const_obj_t native_type);

void mp_obj_print_helper(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
void mp_obj_print(mp_obj_t o, mp_print_kind_t kind);
void mp_obj_print_exception(const mp_print_t *print, mp_obj_t exc);

bool mp_obj_is_true(mp_obj_t arg);
bool mp_obj_is_callable(mp_obj_t o_in);
mp_obj_t mp_obj_equal_not_equal(mp_binary_op_t op, mp_obj_t o1, mp_obj_t o2);
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2);

// returns true if o is bool, small int or long int
static inline bool mp_obj_is_integer(mp_const_obj_t o) {
    return mp_obj_is_int(o) || mp_obj_is_bool(o);
}

mp_int_t mp_obj_get_int(mp_const_obj_t arg);
mp_int_t mp_obj_get_int_truncated(mp_const_obj_t arg);
bool mp_obj_get_int_maybe(mp_const_obj_t arg, mp_int_t *value);
#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mp_obj_get_float(mp_obj_t self_in);
bool mp_obj_get_float_maybe(mp_obj_t arg, mp_float_t *value);
void mp_obj_get_complex(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
bool mp_obj_get_complex_maybe(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
#endif
void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items); // *items may point inside a GC block
void mp_obj_get_array_fixed_n(mp_obj_t o, size_t len, mp_obj_t **items); // *items may point inside a GC block
size_t mp_get_index(const mp_obj_type_t *type, size_t len, mp_obj_t index, bool is_slice);
mp_obj_t mp_obj_id(mp_obj_t o_in);
mp_obj_t mp_obj_len(mp_obj_t o_in);
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in); // may return MP_OBJ_NULL
mp_obj_t mp_obj_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t val);

// cell

typedef struct _mp_obj_cell_t {
    mp_obj_base_t base;
    mp_obj_t obj;
} mp_obj_cell_t;

static inline mp_obj_t mp_obj_cell_get(mp_obj_t self_in) {
    mp_obj_cell_t *self = (mp_obj_cell_t *)MP_OBJ_TO_PTR(self_in);
    return self->obj;
}

static inline void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj) {
    mp_obj_cell_t *self = (mp_obj_cell_t *)MP_OBJ_TO_PTR(self_in);
    self->obj = obj;
}

// int
// For long int, returns value truncated to mp_int_t
mp_int_t mp_obj_int_get_truncated(mp_const_obj_t self_in);
// Will raise exception if value doesn't fit into mp_int_t
mp_int_t mp_obj_int_get_checked(mp_const_obj_t self_in);
// Will raise exception if value is negative or doesn't fit into mp_uint_t
mp_uint_t mp_obj_int_get_uint_checked(mp_const_obj_t self_in);

// exception
bool mp_obj_is_native_exception_instance(mp_obj_t self_in);
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
static inline mp_obj_t mp_obj_new_exception_arg1(const mp_obj_type_t *exc_type, mp_obj_t arg) {
    assert(MP_OBJ_TYPE_GET_SLOT_OR_NULL(exc_type, make_new) == mp_obj_exception_make_new);
    return mp_obj_exception_make_new(exc_type, 1, 0, &arg);
}

// str
bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2);
qstr mp_obj_str_get_qstr(mp_obj_t self_in); // use this if you will anyway convert the string to a qstr
const char *mp_obj_str_get_str(mp_obj_t self_in); // use this only if you need the string to be null terminated
const char *mp_obj_str_get_data(mp_obj_t self_in, size_t *len);
mp_obj_t mp_obj_str_intern(mp_obj_t str);
mp_obj_t mp_obj_str_intern_checked(mp_obj_t obj);
void mp_str_print_quoted(const mp_print_t *print, const byte *str_data, size_t str_len, bool is_bytes);

#if MICROPY_PY_BUILTINS_FLOAT
// float
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
static inline float mp_obj_get_float_to_f(mp_obj_t o) {
    return mp_obj_get_float(o);
}

static inline double mp_obj_get_float_to_d(mp_obj_t o) {
    return (double)mp_obj_get_float(o);
}

static inline mp_obj_t mp_obj_new_float_from_f(float o) {
    return mp_obj_new_float(o);
}

static inline mp_obj_t mp_obj_new_float_from_d(double o) {
    return mp_obj_new_float((mp_float_t)o);
}
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
static inline float mp_obj_get_float_to_f(mp_obj_t o) {
    return (float)mp_obj_get_float(o);
}

static inline double mp_obj_get_float_to_d(mp_obj_t o) {
    return mp_obj_get_float(o);
}

static inline mp_obj_t mp_obj_new_float_from_f(float o) {
    return mp_obj_new_float((mp_float_t)o);
}

static inline mp_obj_t mp_obj_new_float_from_d(double o) {
    return mp_obj_new_float(o);
}
#endif
#if MICROPY_FLOAT_HIGH_QUALITY_HASH
mp_int_t mp_float_hash(mp_float_t val);
#else
static inline mp_int_t mp_float_hash(mp_float_t val) {
    return (mp_int_t)val;
}
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
mp_obj_t mp_obj_dict_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_obj_dict_init(mp_obj_dict_t *dict, size_t n_args);
size_t mp_obj_dict_len(mp_obj_t self_in);
mp_obj_t mp_obj_dict_get(mp_obj_t self_in, mp_obj_t index);
mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
mp_obj_t mp_obj_dict_delete(mp_obj_t self_in, mp_obj_t key);
mp_obj_t mp_obj_dict_copy(mp_obj_t self_in);
static inline mp_map_t *mp_obj_dict_get_map(mp_obj_t dict) {
    return &((mp_obj_dict_t *)MP_OBJ_TO_PTR(dict))->map;
}

// set
void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item);

// slice indexes resolved to particular sequence
typedef struct {
    mp_int_t start;
    mp_int_t stop;
    mp_int_t step;
} mp_bound_slice_t;

// slice
typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;
void mp_obj_slice_indices(mp_obj_t self_in, mp_int_t length, mp_bound_slice_t *result);

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

typedef struct _mp_obj_fun_builtin_var_t {
    mp_obj_base_t base;
    uint32_t sig; // see MP_OBJ_FUN_MAKE_SIG
    union {
        mp_fun_var_t var;
        mp_fun_kw_t kw;
    } fun;
} mp_obj_fun_builtin_var_t;

qstr mp_obj_fun_get_name(mp_const_obj_t fun);

mp_obj_t mp_identity(mp_obj_t self);
MP_DECLARE_CONST_FUN_OBJ_1(mp_identity_obj);

// module
typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;
} mp_obj_module_t;
static inline mp_obj_dict_t *mp_obj_module_get_globals(mp_obj_t module) {
    return ((mp_obj_module_t *)MP_OBJ_TO_PTR(module))->globals;
}

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
mp_obj_t mp_seq_extract_slice(const mp_obj_t *seq, mp_bound_slice_t *indexes);

// Helper to clear stale pointers from allocated, but unused memory, to preclude GC problems
#define mp_seq_clear(start, len, alloc_len, item_sz) memset((byte *)(start) + (len) * (item_sz), 0, ((alloc_len) - (len)) * (item_sz))

// Note: dest and slice regions may overlap
#define mp_seq_replace_slice_no_grow(dest, dest_len, beg, end, slice, slice_len, item_sz) \
    memmove(((char *)dest) + (beg) * (item_sz), slice, slice_len * (item_sz)); \
    memmove(((char *)dest) + (beg + slice_len) * (item_sz), ((char *)dest) + (end) * (item_sz), (dest_len - end) * (item_sz));

// Note: dest and slice regions may overlap
#define mp_seq_replace_slice_grow_inplace(dest, dest_len, beg, end, slice, slice_len, len_adj, item_sz) \
    memmove(((char *)dest) + (beg + slice_len) * (item_sz), ((char *)dest) + (end) * (item_sz), ((dest_len) + (len_adj) - ((beg) + (slice_len))) * (item_sz)); \
    memmove(((char *)dest) + (beg) * (item_sz), slice, slice_len * (item_sz));

#if !MICROPY_PREVIEW_VERSION_2

// Provide translation for legacy API
#define MP_OBJ_IS_SMALL_INT mp_obj_is_small_int
#define MP_OBJ_IS_QSTR mp_obj_is_qstr
#define MP_OBJ_IS_OBJ mp_obj_is_obj
#define MP_OBJ_IS_INT mp_obj_is_int
#define MP_OBJ_IS_TYPE mp_obj_is_type
#define MP_OBJ_IS_STR mp_obj_is_str
#define MP_OBJ_IS_STR_OR_BYTES mp_obj_is_str_or_bytes
#define MP_OBJ_IS_FUN mp_obj_is_fun
#define MP_MAP_SLOT_IS_FILLED mp_map_slot_is_filled
#define MP_SET_SLOT_IS_FILLED mp_set_slot_is_filled

#endif

#endif // MICROPY_INCLUDED_PY_OBJ_H
