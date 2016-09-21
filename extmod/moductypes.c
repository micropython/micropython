/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/binary.h"

#if MICROPY_PY_UCTYPES

/// \module uctypes - Access data structures in memory
///
/// The module allows to define layout of raw data structure (using terms
/// of C language), and then access memory buffers using this definition.
/// The module also provides convenience functions to access memory buffers
/// contained in Python objects or wrap memory buffers in Python objects.
/// \constant UINT8_1 - uint8_t value type

/// \class struct - C-like structure
///
/// Encapsulalation of in-memory data structure. This class doesn't define
/// any methods, only attribute access (for structure fields) and
/// indexing (for pointer and array fields).
///
/// Usage:
///
///     # Define layout of a structure with 2 fields
///     # 0 and 4 are byte offsets of fields from the beginning of struct
///     # they are logically ORed with field type
///     FOO_STRUCT = {"a": 0 | uctypes.UINT32, "b": 4 | uctypes.UINT8}
///
///     # Example memory buffer to access (contained in bytes object)
///     buf = b"\x64\0\0\0\0x14"
///
///     # Create structure object referring to address of
///     # the data in the buffer above
///     s = uctypes.struct(FOO_STRUCT, uctypes.addressof(buf))
///
///     # Access fields
///     print(s.a, s.b)
///     # Result:
///     # 100, 20

#define LAYOUT_LITTLE_ENDIAN (0)
#define LAYOUT_BIG_ENDIAN    (1)
#define LAYOUT_NATIVE        (2)

#define VAL_TYPE_BITS 4
#define BITF_LEN_BITS 5
#define BITF_OFF_BITS 5
#define OFFSET_BITS 17
#if VAL_TYPE_BITS + BITF_LEN_BITS + BITF_OFF_BITS + OFFSET_BITS != 31
#error Invalid encoding field length
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
    STRUCT, PTR, ARRAY, BITFIELD,
};

// Here we need to set sign bit right
#define TYPE2SMALLINT(x, nbits) ((((int)x) << (32 - nbits)) >> 1)
#define GET_TYPE(x, nbits) (((x) >> (31 - nbits)) & ((1 << nbits) - 1))
// Bit 0 is "is_signed"
#define GET_SCALAR_SIZE(val_type) (1 << ((val_type) >> 1))
#define VALUE_MASK(type_nbits) ~((int)0x80000000 >> type_nbits)

#define IS_SCALAR_ARRAY(tuple_desc) ((tuple_desc)->len == 2)
// We cannot apply the below to INT8, as their range [-128, 127]
#define IS_SCALAR_ARRAY_OF_BYTES(tuple_desc) (GET_TYPE(MP_OBJ_SMALL_INT_VALUE((tuple_desc)->items[1]), VAL_TYPE_BITS) == UINT8)

// "struct" in uctypes context means "structural", i.e. aggregate, type.
STATIC const mp_obj_type_t uctypes_struct_type;

typedef struct _mp_obj_uctypes_struct_t {
    mp_obj_base_t base;
    mp_obj_t desc;
    byte *addr;
    uint32_t flags;
} mp_obj_uctypes_struct_t;

STATIC NORETURN void syntax_error(void) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "syntax error in uctypes descriptor"));
}

STATIC mp_obj_t uctypes_struct_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 3, false);
    mp_obj_uctypes_struct_t *o = m_new_obj(mp_obj_uctypes_struct_t);
    o->base.type = type;
    o->addr = (void*)(uintptr_t)mp_obj_int_get_truncated(args[0]);
    o->desc = args[1];
    o->flags = LAYOUT_NATIVE;
    if (n_args == 3) {
        o->flags = mp_obj_get_int(args[2]);
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC void uctypes_struct_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);
    const char *typen = "unk";
    if (MP_OBJ_IS_TYPE(self->desc, &mp_type_dict)) {
        typen = "STRUCT";
    } else if (MP_OBJ_IS_TYPE(self->desc, &mp_type_tuple)) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->desc);
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
        uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
        switch (agg_type) {
            case PTR: typen = "PTR"; break;
            case ARRAY: typen = "ARRAY"; break;
        }
    } else {
        typen = "ERROR";
    }
    mp_printf(print, "<struct %s %p>", typen, self->addr);
}

// Get size of any type descriptor
STATIC mp_uint_t uctypes_struct_size(mp_obj_t desc_in, int layout_type, mp_uint_t *max_field_size);

// Get size of scalar type descriptor
static inline mp_uint_t uctypes_struct_scalar_size(int val_type) {
    if (val_type == FLOAT32) {
        return 4;
    } else {
        return GET_SCALAR_SIZE(val_type & 7);
    }
}

// Get size of aggregate type descriptor
STATIC mp_uint_t uctypes_struct_agg_size(mp_obj_tuple_t *t, int layout_type, mp_uint_t *max_field_size) {
    mp_uint_t total_size = 0;

    mp_int_t offset_ = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
    mp_uint_t agg_type = GET_TYPE(offset_, AGG_TYPE_BITS);

    switch (agg_type) {
        case STRUCT:
            return uctypes_struct_size(t->items[1], layout_type, max_field_size);
        case PTR:
            if (sizeof(void*) > *max_field_size) {
                *max_field_size = sizeof(void*);
            }
            return sizeof(void*);
        case ARRAY: {
            mp_int_t arr_sz = MP_OBJ_SMALL_INT_VALUE(t->items[1]);
            uint val_type = GET_TYPE(arr_sz, VAL_TYPE_BITS);
            arr_sz &= VALUE_MASK(VAL_TYPE_BITS);
            mp_uint_t item_s;
            if (t->len == 2) {
                // Elements of array are scalar
                item_s = GET_SCALAR_SIZE(val_type);
                if (item_s > *max_field_size) {
                    *max_field_size = item_s;
                }
            } else {
                // Elements of array are aggregates
                item_s = uctypes_struct_size(t->items[2], layout_type, max_field_size);
            }

            return item_s * arr_sz;
        }
        default:
            assert(0);
    }

    return total_size;
}

STATIC mp_uint_t uctypes_struct_size(mp_obj_t desc_in, int layout_type, mp_uint_t *max_field_size) {
    if (!MP_OBJ_IS_TYPE(desc_in, &mp_type_dict)) {
        if (MP_OBJ_IS_TYPE(desc_in, &mp_type_tuple)) {
            return uctypes_struct_agg_size((mp_obj_tuple_t*)MP_OBJ_TO_PTR(desc_in), layout_type, max_field_size);
        } else if (MP_OBJ_IS_SMALL_INT(desc_in)) {
            // We allow sizeof on both type definitions and structures/structure fields,
            // but scalar structure field is lowered into native Python int, so all
            // type info is lost. So, we cannot say if it's scalar type description,
            // or such lowered scalar.
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "Cannot unambiguously get sizeof scalar"));
        }
        syntax_error();
    }

    mp_obj_dict_t *d = MP_OBJ_TO_PTR(desc_in);
    mp_uint_t total_size = 0;

    for (mp_uint_t i = 0; i < d->map.alloc; i++) {
        if (MP_MAP_SLOT_IS_FILLED(&d->map, i)) {
            mp_obj_t v = d->map.table[i].value;
            if (MP_OBJ_IS_SMALL_INT(v)) {
                mp_uint_t offset = MP_OBJ_SMALL_INT_VALUE(v);
                mp_uint_t val_type = GET_TYPE(offset, VAL_TYPE_BITS);
                offset &= VALUE_MASK(VAL_TYPE_BITS);
                if (val_type >= BFUINT8 && val_type <= BFINT32) {
                    offset &= (1 << OFFSET_BITS) - 1;
                }
                mp_uint_t s = uctypes_struct_scalar_size(val_type);
                if (s > *max_field_size) {
                    *max_field_size = s;
                }
                if (offset + s > total_size) {
                    total_size = offset + s;
                }
            } else {
                if (!MP_OBJ_IS_TYPE(v, &mp_type_tuple)) {
                    syntax_error();
                }
                mp_obj_tuple_t *t = MP_OBJ_TO_PTR(v);
                mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
                offset &= VALUE_MASK(AGG_TYPE_BITS);
                mp_uint_t s = uctypes_struct_agg_size(t, layout_type, max_field_size);
                if (offset + s > total_size) {
                    total_size = offset + s;
                }
            }
        }
    }

    // Round size up to alignment of biggest field
    if (layout_type == LAYOUT_NATIVE) {
        total_size = (total_size + *max_field_size - 1) & ~(*max_field_size - 1);
    }
    return total_size;
}

STATIC mp_obj_t uctypes_struct_sizeof(mp_obj_t obj_in) {
    mp_uint_t max_field_size = 0;
    if (MP_OBJ_IS_TYPE(obj_in, &mp_type_bytearray)) {
        return mp_obj_len(obj_in);
    }
    int layout_type = LAYOUT_NATIVE;
    // We can apply sizeof either to structure definition (a dict)
    // or to instantiated structure
    if (MP_OBJ_IS_TYPE(obj_in, &uctypes_struct_type)) {
        // Extract structure definition
        mp_obj_uctypes_struct_t *obj = MP_OBJ_TO_PTR(obj_in);
        obj_in = obj->desc;
        layout_type = obj->flags;
    }
    mp_uint_t size = uctypes_struct_size(obj_in, layout_type, &max_field_size);
    return MP_OBJ_NEW_SMALL_INT(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uctypes_struct_sizeof_obj, uctypes_struct_sizeof);

STATIC inline mp_obj_t get_unaligned(uint val_type, byte *p, int big_endian) {
    char struct_type = big_endian ? '>' : '<';
    static const char type2char[16] = "BbHhIiQq------fd";
    return mp_binary_get_val(struct_type, type2char[val_type], &p);
}

STATIC inline void set_unaligned(uint val_type, byte *p, int big_endian, mp_obj_t val) {
    char struct_type = big_endian ? '>' : '<';
    static const char type2char[16] = "BbHhIiQq------fd";
    mp_binary_set_val(struct_type, type2char[val_type], val, &p);
}

static inline mp_uint_t get_aligned_basic(uint val_type, void *p) {
    switch (val_type) {
        case UINT8:
            return *(uint8_t*)p;
        case UINT16:
            return *(uint16_t*)p;
        case UINT32:
            return *(uint32_t*)p;
    }
    assert(0);
    return 0;
}

static inline void set_aligned_basic(uint val_type, void *p, mp_uint_t v) {
    switch (val_type) {
        case UINT8:
            *(uint8_t*)p = (uint8_t)v; return;
        case UINT16:
            *(uint16_t*)p = (uint16_t)v; return;
        case UINT32:
            *(uint32_t*)p = (uint32_t)v; return;
    }
    assert(0);
}

STATIC mp_obj_t get_aligned(uint val_type, void *p, mp_int_t index) {
    switch (val_type) {
        case UINT8:
            return MP_OBJ_NEW_SMALL_INT(((uint8_t*)p)[index]);
        case INT8:
            return MP_OBJ_NEW_SMALL_INT(((int8_t*)p)[index]);
        case UINT16:
            return MP_OBJ_NEW_SMALL_INT(((uint16_t*)p)[index]);
        case INT16:
            return MP_OBJ_NEW_SMALL_INT(((int16_t*)p)[index]);
        case UINT32:
            return mp_obj_new_int_from_uint(((uint32_t*)p)[index]);
        case INT32:
            return mp_obj_new_int(((int32_t*)p)[index]);
        case UINT64:
            return mp_obj_new_int_from_ull(((uint64_t*)p)[index]);
        case INT64:
            return mp_obj_new_int_from_ll(((int64_t*)p)[index]);
        #if MICROPY_PY_BUILTINS_FLOAT
        case FLOAT32:
            return mp_obj_new_float(((float*)p)[index]);
        case FLOAT64:
            return mp_obj_new_float(((double*)p)[index]);
        #endif
        default:
            assert(0);
            return MP_OBJ_NULL;
    }
}

STATIC void set_aligned(uint val_type, void *p, mp_int_t index, mp_obj_t val) {
    #if MICROPY_PY_BUILTINS_FLOAT
    if (val_type == FLOAT32 || val_type == FLOAT64) {
        mp_float_t v = mp_obj_get_float(val);
        if (val_type == FLOAT32) {
            ((float*)p)[index] = v;
        } else {
            ((double*)p)[index] = v;
        }
        return;
    }
    #endif
    mp_int_t v = mp_obj_get_int(val);
    switch (val_type) {
        case UINT8:
            ((uint8_t*)p)[index] = (uint8_t)v; return;
        case INT8:
            ((int8_t*)p)[index] = (int8_t)v; return;
        case UINT16:
            ((uint16_t*)p)[index] = (uint16_t)v; return;
        case INT16:
            ((int16_t*)p)[index] = (int16_t)v; return;
        case UINT32:
            ((uint32_t*)p)[index] = (uint32_t)v; return;
        case INT32:
            ((int32_t*)p)[index] = (int32_t)v; return;
        case INT64:
        case UINT64:
            if (sizeof(mp_int_t) == 8) {
                ((uint64_t*)p)[index] = (uint64_t)v;
            } else {
                // TODO: Doesn't offer atomic store semantics, but should at least try
                set_unaligned(val_type, p, MP_ENDIANNESS_BIG, val);
            }
            return;
        default:
            assert(0);
    }
}

STATIC mp_obj_t uctypes_struct_attr_op(mp_obj_t self_in, qstr attr, mp_obj_t set_val) {
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);

    // TODO: Support at least OrderedDict in addition
    if (!MP_OBJ_IS_TYPE(self->desc, &mp_type_dict)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "struct: no fields"));
    }

    mp_obj_t deref = mp_obj_dict_get(self->desc, MP_OBJ_NEW_QSTR(attr));
    if (MP_OBJ_IS_SMALL_INT(deref)) {
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(deref);
        mp_uint_t val_type = GET_TYPE(offset, VAL_TYPE_BITS);
        offset &= VALUE_MASK(VAL_TYPE_BITS);
//printf("scalar type=%d offset=%x\n", val_type, offset);

        if (val_type <= INT64 || val_type == FLOAT32 || val_type == FLOAT64) {
//            printf("size=%d\n", GET_SCALAR_SIZE(val_type));
            if (self->flags == LAYOUT_NATIVE) {
                if (set_val == MP_OBJ_NULL) {
                    return get_aligned(val_type, self->addr + offset, 0);
                } else {
                    set_aligned(val_type, self->addr + offset, 0, set_val);
                    return set_val; // just !MP_OBJ_NULL
                }
            } else {
                if (set_val == MP_OBJ_NULL) {
                    return get_unaligned(val_type, self->addr + offset, self->flags);
                } else {
                    set_unaligned(val_type, self->addr + offset, self->flags, set_val);
                    return set_val; // just !MP_OBJ_NULL
                }
            }
        } else if (val_type >= BFUINT8 && val_type <= BFINT32) {
            uint bit_offset = (offset >> 17) & 31;
            uint bit_len = (offset >> 22) & 31;
            offset &= (1 << 17) - 1;
            mp_uint_t val;
            if (self->flags == LAYOUT_NATIVE) {
                val = get_aligned_basic(val_type & 6, self->addr + offset);
            } else {
                val = mp_binary_get_int(GET_SCALAR_SIZE(val_type & 7), val_type & 1, self->flags, self->addr + offset);
            }
            if (set_val == MP_OBJ_NULL) {
                val >>= bit_offset;
                val &= (1 << bit_len) - 1;
                // TODO: signed
                assert((val_type & 1) == 0);
                return mp_obj_new_int(val);
            } else {
                mp_uint_t set_val_int = (mp_uint_t)mp_obj_get_int(set_val);
                mp_uint_t mask = (1 << bit_len) - 1;
                set_val_int &= mask;
                set_val_int <<= bit_offset;
                mask <<= bit_offset;
                val = (val & ~mask) | set_val_int;

                if (self->flags == LAYOUT_NATIVE) {
                    set_aligned_basic(val_type & 6, self->addr + offset, val);
                } else {
                    mp_binary_set_int(GET_SCALAR_SIZE(val_type & 7), self->flags == LAYOUT_BIG_ENDIAN,
                        self->addr + offset, val);
                }
                return set_val; // just !MP_OBJ_NULL
            }
        }

        assert(0);
        return MP_OBJ_NULL;
    }

    if (!MP_OBJ_IS_TYPE(deref, &mp_type_tuple)) {
        syntax_error();
    }

    if (set_val != MP_OBJ_NULL) {
        // Cannot assign to aggregate
        syntax_error();
    }

    mp_obj_tuple_t *sub = MP_OBJ_TO_PTR(deref);
    mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(sub->items[0]);
    mp_uint_t agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
    offset &= VALUE_MASK(AGG_TYPE_BITS);
//printf("agg type=%d offset=%x\n", agg_type, offset);

    switch (agg_type) {
        case STRUCT: {
            mp_obj_uctypes_struct_t *o = m_new_obj(mp_obj_uctypes_struct_t);
            o->base.type = &uctypes_struct_type;
            o->desc = sub->items[1];
            o->addr = self->addr + offset;
            o->flags = self->flags;
            return MP_OBJ_FROM_PTR(o);
        }
        case ARRAY: {
            mp_uint_t dummy;
            if (IS_SCALAR_ARRAY(sub) && IS_SCALAR_ARRAY_OF_BYTES(sub)) {
                return mp_obj_new_bytearray_by_ref(uctypes_struct_agg_size(sub, self->flags, &dummy), self->addr + offset);
            }
            // Fall thru to return uctypes struct object
        }
        case PTR: {
            mp_obj_uctypes_struct_t *o = m_new_obj(mp_obj_uctypes_struct_t);
            o->base.type = &uctypes_struct_type;
            o->desc = MP_OBJ_FROM_PTR(sub);
            o->addr = self->addr + offset;
            o->flags = self->flags;
//printf("PTR/ARR base addr=%p\n", o->addr);
            return MP_OBJ_FROM_PTR(o);
        }
    }

    // Should be unreachable once all cases are handled
    return MP_OBJ_NULL;
}

STATIC void uctypes_struct_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_t val = uctypes_struct_attr_op(self_in, attr, MP_OBJ_NULL);
        dest[0] = val;
    } else {
        // delete/store attribute
        if (uctypes_struct_attr_op(self_in, attr, dest[1]) != MP_OBJ_NULL) {
            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC mp_obj_t uctypes_struct_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);

    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    } else {
        // load / store
        if (!MP_OBJ_IS_TYPE(self->desc, &mp_type_tuple)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "struct: cannot index"));
        }

        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->desc);
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
        uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);

        mp_int_t index = MP_OBJ_SMALL_INT_VALUE(index_in);

        if (agg_type == ARRAY) {
            mp_int_t arr_sz = MP_OBJ_SMALL_INT_VALUE(t->items[1]);
            uint val_type = GET_TYPE(arr_sz, VAL_TYPE_BITS);
            arr_sz &= VALUE_MASK(VAL_TYPE_BITS);
            if (index >= arr_sz) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "struct: index out of range"));
            }

            if (t->len == 2) {
                // array of scalars
                if (self->flags == LAYOUT_NATIVE) {
                    if (value == MP_OBJ_SENTINEL) {
                        return get_aligned(val_type, self->addr, index);
                    } else {
                        set_aligned(val_type, self->addr, index, value);
                        return value; // just !MP_OBJ_NULL
                    }
                } else {
                    byte *p = self->addr + GET_SCALAR_SIZE(val_type) * index;
                    if (value == MP_OBJ_SENTINEL) {
                        return get_unaligned(val_type, p, self->flags);
                    } else {
                        set_unaligned(val_type, p, self->flags, value);
                        return value; // just !MP_OBJ_NULL
                    }
                }
            } else if (value == MP_OBJ_SENTINEL) {
                mp_uint_t dummy = 0;
                mp_uint_t size = uctypes_struct_size(t->items[2], self->flags, &dummy);
                mp_obj_uctypes_struct_t *o = m_new_obj(mp_obj_uctypes_struct_t);
                o->base.type = &uctypes_struct_type;
                o->desc = t->items[2];
                o->addr = self->addr + size * index;
                o->flags = self->flags;
                return MP_OBJ_FROM_PTR(o);
            } else {
                return MP_OBJ_NULL; // op not supported
            }

        } else if (agg_type == PTR) {
            byte *p = *(void**)self->addr;
            if (MP_OBJ_IS_SMALL_INT(t->items[1])) {
                uint val_type = GET_TYPE(MP_OBJ_SMALL_INT_VALUE(t->items[1]), VAL_TYPE_BITS);
                return get_aligned(val_type, p, index);
            } else {
                mp_uint_t dummy = 0;
                mp_uint_t size = uctypes_struct_size(t->items[1], self->flags, &dummy);
                mp_obj_uctypes_struct_t *o = m_new_obj(mp_obj_uctypes_struct_t);
                o->base.type = &uctypes_struct_type;
                o->desc = t->items[1];
                o->addr = p + size * index;
                o->flags = self->flags;
                return MP_OBJ_FROM_PTR(o);
            }
        }

        assert(0);
        return MP_OBJ_NULL;
    }
}

STATIC mp_int_t uctypes_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t max_field_size = 0;
    mp_uint_t size = uctypes_struct_size(self->desc, self->flags, &max_field_size);

    bufinfo->buf = self->addr;
    bufinfo->len = size;
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

/// \function addressof()
/// Return address of object's data (applies to object providing buffer
/// interface).
STATIC mp_obj_t uctypes_struct_addressof(mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    return mp_obj_new_int((mp_int_t)(uintptr_t)bufinfo.buf);
}
MP_DEFINE_CONST_FUN_OBJ_1(uctypes_struct_addressof_obj, uctypes_struct_addressof);

/// \function bytearray_at()
/// Capture memory at given address of given size as bytearray. Memory is
/// captured by reference (and thus memory pointed by bytearray may change
/// or become invalid at later time). Use bytes_at() to capture by value.
STATIC mp_obj_t uctypes_struct_bytearray_at(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytearray_by_ref(mp_obj_int_get_truncated(size), (void*)(uintptr_t)mp_obj_int_get_truncated(ptr));
}
MP_DEFINE_CONST_FUN_OBJ_2(uctypes_struct_bytearray_at_obj, uctypes_struct_bytearray_at);

/// \function bytes_at()
/// Capture memory at given address of given size as bytes. Memory is
/// captured by value, i.e. copied. Use bytearray_at() to capture by reference
/// ("zero copy").
STATIC mp_obj_t uctypes_struct_bytes_at(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytes((void*)(uintptr_t)mp_obj_int_get_truncated(ptr), mp_obj_int_get_truncated(size));
}
MP_DEFINE_CONST_FUN_OBJ_2(uctypes_struct_bytes_at_obj, uctypes_struct_bytes_at);


STATIC const mp_obj_type_t uctypes_struct_type = {
    { &mp_type_type },
    .name = MP_QSTR_struct,
    .print = uctypes_struct_print,
    .make_new = uctypes_struct_make_new,
    .attr = uctypes_struct_attr,
    .subscr = uctypes_struct_subscr,
    .buffer_p = { .get_buffer = uctypes_get_buffer },
};

STATIC const mp_rom_map_elem_t mp_module_uctypes_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uctypes) },
    { MP_ROM_QSTR(MP_QSTR_struct), MP_ROM_PTR(&uctypes_struct_type) },
    { MP_ROM_QSTR(MP_QSTR_sizeof), MP_ROM_PTR(&uctypes_struct_sizeof_obj) },
    { MP_ROM_QSTR(MP_QSTR_addressof), MP_ROM_PTR(&uctypes_struct_addressof_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes_at), MP_ROM_PTR(&uctypes_struct_bytes_at_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytearray_at), MP_ROM_PTR(&uctypes_struct_bytearray_at_obj) },

    /// \moduleref uctypes

    /// \constant NATIVE - Native structure layout - native endianness,
    /// platform-specific field alignment
    { MP_ROM_QSTR(MP_QSTR_NATIVE), MP_ROM_INT(LAYOUT_NATIVE) },
    /// \constant LITTLE_ENDIAN - Little-endian structure layout, tightly packed
    /// (no alignment constraints)
    { MP_ROM_QSTR(MP_QSTR_LITTLE_ENDIAN), MP_ROM_INT(LAYOUT_LITTLE_ENDIAN) },
    /// \constant BIG_ENDIAN - Big-endian structure layout, tightly packed
    /// (no alignment constraints)
    { MP_ROM_QSTR(MP_QSTR_BIG_ENDIAN), MP_ROM_INT(LAYOUT_BIG_ENDIAN) },

    /// \constant VOID - void value type, may be used only as pointer target type.
    { MP_ROM_QSTR(MP_QSTR_VOID), MP_ROM_INT(TYPE2SMALLINT(UINT8, VAL_TYPE_BITS)) },

    /// \constant UINT8 - uint8_t value type
    { MP_ROM_QSTR(MP_QSTR_UINT8), MP_ROM_INT(TYPE2SMALLINT(UINT8, 4)) },
    /// \constant INT8 - int8_t value type
    { MP_ROM_QSTR(MP_QSTR_INT8), MP_ROM_INT(TYPE2SMALLINT(INT8, 4)) },
    /// \constant UINT16 - uint16_t value type
    { MP_ROM_QSTR(MP_QSTR_UINT16), MP_ROM_INT(TYPE2SMALLINT(UINT16, 4)) },
    /// \constant INT16 - int16_t value type
    { MP_ROM_QSTR(MP_QSTR_INT16), MP_ROM_INT(TYPE2SMALLINT(INT16, 4)) },
    /// \constant UINT32 - uint32_t value type
    { MP_ROM_QSTR(MP_QSTR_UINT32), MP_ROM_INT(TYPE2SMALLINT(UINT32, 4)) },
    /// \constant INT32 - int32_t value type
    { MP_ROM_QSTR(MP_QSTR_INT32), MP_ROM_INT(TYPE2SMALLINT(INT32, 4)) },
    /// \constant UINT64 - uint64_t value type
    { MP_ROM_QSTR(MP_QSTR_UINT64), MP_ROM_INT(TYPE2SMALLINT(UINT64, 4)) },
    /// \constant INT64 - int64_t value type
    { MP_ROM_QSTR(MP_QSTR_INT64), MP_ROM_INT(TYPE2SMALLINT(INT64, 4)) },

    { MP_ROM_QSTR(MP_QSTR_BFUINT8), MP_ROM_INT(TYPE2SMALLINT(BFUINT8, 4)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT8), MP_ROM_INT(TYPE2SMALLINT(BFINT8, 4)) },
    { MP_ROM_QSTR(MP_QSTR_BFUINT16), MP_ROM_INT(TYPE2SMALLINT(BFUINT16, 4)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT16), MP_ROM_INT(TYPE2SMALLINT(BFINT16, 4)) },
    { MP_ROM_QSTR(MP_QSTR_BFUINT32), MP_ROM_INT(TYPE2SMALLINT(BFUINT32, 4)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT32), MP_ROM_INT(TYPE2SMALLINT(BFINT32, 4)) },

    { MP_ROM_QSTR(MP_QSTR_BF_POS), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_BF_LEN), MP_ROM_INT(22) },

    #if MICROPY_PY_BUILTINS_FLOAT
    { MP_ROM_QSTR(MP_QSTR_FLOAT32), MP_ROM_INT(TYPE2SMALLINT(FLOAT32, 4)) },
    { MP_ROM_QSTR(MP_QSTR_FLOAT64), MP_ROM_INT(TYPE2SMALLINT(FLOAT64, 4)) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_PTR), MP_ROM_INT(TYPE2SMALLINT(PTR, AGG_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_ARRAY), MP_ROM_INT(TYPE2SMALLINT(ARRAY, AGG_TYPE_BITS)) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_uctypes_globals, mp_module_uctypes_globals_table);

const mp_obj_module_t mp_module_uctypes = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_uctypes_globals,
};

#endif
