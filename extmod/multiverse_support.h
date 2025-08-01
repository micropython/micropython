#pragma once
#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/moductypes.h"

#if __m68k__
#include "Multiverse.h"
#else
typedef long LONGINT;
typedef short INTEGER;
typedef unsigned char Byte;
typedef struct { INTEGER h, v;
} Point;
#define pascal /* nothing */
#endif

// Relies on gcc Variadic Macros and Statement Expressions
#define NEW_TUPLE(...) \
    ({mp_obj_t _z[] = {__VA_ARGS__}; mp_obj_new_tuple(MP_ARRAY_SIZE(_z), _z); })

#define ROM_TUPLE(...) \
    {{&mp_type_tuple}, MP_ARRAY_SIZE(((const mp_obj_t[]) {__VA_ARGS__})), {__VA_ARGS__}}

#define DECLARE_PTR_SCALAR(type_c) \
    MP_DECLARE_CTYPES_STRUCT(type_c##_obj)

DECLARE_PTR_SCALAR(bool);
DECLARE_PTR_SCALAR(char);
DECLARE_PTR_SCALAR(uint8_t);
DECLARE_PTR_SCALAR(uint16_t);
DECLARE_PTR_SCALAR(uint32_t);
DECLARE_PTR_SCALAR(uint64_t);
DECLARE_PTR_SCALAR(int8_t);
DECLARE_PTR_SCALAR(int16_t);
DECLARE_PTR_SCALAR(int32_t);
DECLARE_PTR_SCALAR(int64_t);

#define void_obj uint8_t_obj

void *void_ptr_from_py(mp_obj_t obj);
void *to_struct_helper(mp_obj_t obj, const mp_obj_type_t *struct_type, bool is_const, qstr fieldname);
mp_obj_t from_struct_helper(void *buf, const mp_obj_type_t *type);
void *to_scalar_helper(mp_obj_t obj, size_t objsize, bool is_const);
mp_obj_t from_scalar_helper(void *buf, size_t objsize, bool is_signed_hint);
mp_obj_t LMGet_common(long address, size_t objsize, mp_obj_t arg);
void LMSet_common(long address, size_t objsize, mp_obj_t arg);
Point Point_to_c(mp_obj_t obj, qstr fieldname);
