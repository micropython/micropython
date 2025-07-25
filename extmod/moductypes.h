/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
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

#ifndef MICROPY_INCLUDED_PY_MODUCTYPES_H
#define MICROPY_INCLUDED_PY_MODUCTYPES_H

#if MICROPY_PY_UCTYPES
#include "py/obj.h"

extern const mp_obj_type_t uctypes_struct_type;

typedef struct _mp_obj_ctypes_struct_type_t {
    // This is a mp_obj_type_t with six slots.
    mp_obj_empty_type_t base;
    void *slots[6];
    mp_obj_t desc;
    uint32_t struct_flags;
} mp_obj_ctypes_struct_type_t;

void uctypes_struct_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
void uctypes_struct_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
mp_obj_t uctypes_struct_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value);
mp_obj_t uctypes_struct_unary_op(mp_unary_op_t op, mp_obj_t self_in);
mp_int_t uctypes_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
mp_obj_t uctypes_struct_type_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t uctypes_struct_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

#define MP_DECLARE_CTYPES_STRUCT(type_name) \
    extern const mp_obj_ctypes_struct_type_t type_name;

#define MP_DEFINE_CTYPES_STRUCT(type_name, name_, desc_, flags_) \
    const mp_obj_ctypes_struct_type_t type_name = { \
        .base = { \
            .base = { &mp_type_type }, \
            .flags = MP_TYPE_FLAG_NONE,  \
            .name = name_,  \
            .slot_index_make_new = 1, \
            .slot_index_print = 2, \
            .slot_index_attr = 3, \
            .slot_index_subscr = 4, \
            .slot_index_unary_op = 5, \
            .slot_index_buffer = 6, \
        }, \
        .slots = { \
            uctypes_struct_type_make_new, \
            uctypes_struct_print, \
            uctypes_struct_attr, \
            uctypes_struct_subscr, \
            uctypes_struct_unary_op, \
            uctypes_get_buffer \
        }, \
        .desc = desc_, \
        .struct_flags = flags_, \
    }

// The uctypes module allows defining the layout of a raw data structure (using
// terms of the C language), and then access memory buffers using this definition.
// The module also provides convenience functions to access memory buffers
// contained in Python objects or wrap memory buffers in Python objects.

#define LAYOUT_LITTLE_ENDIAN (0)
#define LAYOUT_BIG_ENDIAN    (1)
#define LAYOUT_NATIVE        (2)

#define LEN_BITS (OFFSET_BITS + BITF_OFF_BITS)
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
#define VAL_TYPE_BITS 4
#define BITF_LEN_BITS 5
#define BITF_OFF_BITS 5
#define OFFSET_BITS 16
#if VAL_TYPE_BITS + BITF_LEN_BITS + BITF_OFF_BITS + OFFSET_BITS != 30
#error Invalid encoding field length
#endif
// Here we need to set sign bit right
#define TYPE2SMALLINT(x, nbits) ((((int)x) << (32 - nbits)) >> 2)
#define GET_TYPE(x, nbits) (((x) >> (30 - nbits)) & ((1 << nbits) - 1))
#define VALUE_MASK(type_nbits) (~((int)0xc0000000 >> type_nbits))
#else
#define VAL_TYPE_BITS 4
#define BITF_LEN_BITS 5
#define BITF_OFF_BITS 5
#define OFFSET_BITS 17
#define LEN_BITS (OFFSET_BITS + BITF_OFF_BITS)
#if VAL_TYPE_BITS + BITF_LEN_BITS + BITF_OFF_BITS + OFFSET_BITS != 31
#error Invalid encoding field length
#endif
// Here we need to set sign bit right
#define TYPE2SMALLINT(x, nbits) ((((mp_int_t)x) << (32 - nbits)) >> 1)
#define GET_TYPE(x, nbits) (((x) >> (31 - nbits)) & ((1 << nbits) - 1))
#define VALUE_MASK(type_nbits) (~((int)0x80000000 >> type_nbits))
#endif

enum {
    UINT8, INT8, UINT16, INT16,
    UINT32, INT32, UINT64, INT64,

    BFUINT8, BFINT8, BFUINT16, BFINT16,
    BFUINT32, BFINT32,

    FLOAT32, FLOAT64,
};

#define AGG_TYPE_BITS 2

enum {
    STRUCT, PTR, ARRAY,
};

// Here we need to set sign bit right
#define UCTYPE_TYPE(t) (TYPE2SMALLINT((t), VAL_TYPE_BITS))
#define UCTYPE_AGG(a) (TYPE2SMALLINT((a), AGG_TYPE_BITS))
#define TYPE_AND_OFFSET(t, o) (UCTYPE_TYPE((t)) | (o))

#endif
#endif
