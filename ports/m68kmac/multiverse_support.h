#pragma once
#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/moductypes.h"
#include <Multiverse.h>

// Relies on gcc Variadic Macros and Statement Expressions
#define NEW_TUPLE(...) \
    ({mp_obj_t _z[] = {__VA_ARGS__}; mp_obj_new_tuple(MP_ARRAY_SIZE(_z), _z); })

#define ROM_TUPLE(...) \
    {{&mp_type_tuple}, MP_ARRAY_SIZE(((mp_obj_t[]) {__VA_ARGS__})), {__VA_ARGS__}}

void *to_struct_helper(mp_obj_t obj, const mp_obj_type_t *struct_type, bool is_const);
mp_obj_t from_struct_helper(void *buf, const mp_obj_type_t *type);
void *to_scalar_helper(mp_obj_t obj, size_t objsize, bool is_const);
mp_obj_t from_scalar_helper(void *buf, size_t objsize, bool is_signed_hint);
mp_obj_t LMGet_common(long address, size_t objsize, mp_obj_t arg);
void LMSet_common(long address, size_t objsize, mp_obj_t arg);
Point Point_to_c(mp_obj_t obj);
