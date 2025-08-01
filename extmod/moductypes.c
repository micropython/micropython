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

#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "extmod/moductypes.h"

#if MICROPY_PY_UCTYPES

// Bit 0 is "is_signed"
#define GET_SCALAR_SIZE(val_type) (1 << ((val_type) >> 1))

#define IS_SCALAR_ARRAY(tuple_desc) ((tuple_desc)->len == 2)
// We cannot apply the below to INT8, as their range [-128, 127]
#define IS_SCALAR_ARRAY_OF_BYTES(tuple_desc) (GET_TYPE(MP_OBJ_SMALL_INT_VALUE((tuple_desc)->items[1]), VAL_TYPE_BITS) == UINT8)

// "struct" in uctypes context means "structural", i.e. aggregate, type.
const mp_obj_type_t uctypes_struct_type;

// Get size of any type descriptor
static mp_uint_t uctypes_struct_size(mp_obj_t desc_in, int layout_type, mp_uint_t *max_field_size);

static mp_obj_t uctypes_struct_attr_op(mp_obj_t self_in, qstr attr, mp_obj_t set_val);

#define CTYPES_FLAGS_SIZE_BITS (2)
#define CTYPES_OFFSET_SIZE_BITS (8 * sizeof(uint32_t) - 2)

typedef struct _mp_obj_uctypes_struct_t {
    mp_obj_base_t base;
    mp_obj_t desc;
    byte *ptrbase;
    uint32_t flags : CTYPES_FLAGS_SIZE_BITS;
    uint32_t offset : CTYPES_OFFSET_SIZE_BITS;
} mp_obj_uctypes_struct_t;

static inline byte *struct_addr(mp_obj_uctypes_struct_t *s) {
    return s->ptrbase + s->offset;
}

static MP_NORETURN void syntax_error(void) {
    mp_raise_TypeError(MP_ERROR_TEXT("syntax error in uctypes descriptor"));
}

static bool is_struct_type(mp_obj_t obj_in) {
    if (!mp_obj_is_type(obj_in, &mp_type_type)) {
        return false;
    }
    mp_make_new_fun_t make_new = MP_OBJ_TYPE_GET_SLOT_OR_NULL((mp_obj_type_t *)MP_OBJ_TO_PTR(obj_in), make_new);
    return make_new == uctypes_struct_type_make_new;
}

static bool is_struct_instance(mp_obj_t obj_in) {
    const mp_obj_type_t *type = mp_obj_get_type(obj_in);
    return MP_OBJ_TYPE_GET_SLOT_OR_NULL(type, subscr) == uctypes_struct_subscr;
}

mp_obj_t uctypes_struct_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 3, false);
    mp_obj_t desc = args[1];
    int flags = LAYOUT_NATIVE;
    if (is_struct_type(desc)) {
        type = MP_OBJ_TO_PTR(desc);
        mp_obj_ctypes_struct_type_t *struct_type = (mp_obj_ctypes_struct_type_t *)type;
        desc = struct_type->desc;
    } else if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    }
    mp_buffer_info_t bufinfo;
    mp_obj_uctypes_struct_t *o = mp_obj_malloc(mp_obj_uctypes_struct_t, type);
    if (mp_get_buffer(args[0], &bufinfo, MP_BUFFER_READ | MP_BUFFER_GET_BASE)) {
        o->ptrbase = bufinfo.base;
        o->offset = (char *)bufinfo.buf - (char *)bufinfo.base;
    } else {
        o->ptrbase = (void *)(uintptr_t)mp_obj_get_int_truncated(args[0]);
        o->offset = 0;
    }
    o->desc = desc;
    o->flags = flags;
    return MP_OBJ_FROM_PTR(o);
}

static bool is_ptr(mp_obj_t desc) {
    if (!mp_obj_is_type(desc, &mp_type_tuple)) {
        return false;
    }
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(desc);
    mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
    uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
    return agg_type == PTR;
}

mp_obj_t uctypes_struct_type_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_ctypes_struct_type_t *type = (mp_obj_ctypes_struct_type_t *)type_in;
    mp_obj_t desc = type->desc;
    mp_uint_t max_field_size = 0;
    mp_uint_t size = uctypes_struct_size(desc, type->struct_flags, &max_field_size);

    mp_buffer_info_t bufinfo;
    if (is_ptr(desc)) {
        bufinfo.len = size;
        if (n_args != 1) {
            mp_raise_TypeError(NULL);
        }
        if (mp_obj_is_int(args[0])) {
            void *ptr = (void *)(uintptr_t)mp_obj_get_int_truncated(args[0]);
            *(void **)bufinfo.buf = ptr;
        } else {
            mp_buffer_info_t pointee;
            mp_get_buffer_raise(args[0], &pointee, MP_BUFFER_WRITE);
            *(void **)bufinfo.buf = pointee.buf;
        }
    } else {
        mp_obj_t bytearray = mp_obj_new_bytearray(size, NULL);
        mp_get_buffer_raise(bytearray, &bufinfo, MP_BUFFER_WRITE);
    }

    mp_obj_t args1[] = {mp_obj_new_int((intptr_t)bufinfo.buf), desc, mp_obj_new_int(type->struct_flags)};
    mp_obj_t result = uctypes_struct_make_new(type_in, 3, 0, args1);

    if (mp_obj_is_type(desc, &mp_type_tuple)) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(desc);
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
        uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
        if (agg_type == ARRAY) {
            for (size_t i = 0; i < n_args; i++) {
                mp_obj_subscr(result, mp_obj_new_int(i), args[i]);
            }
        } else if (agg_type != PTR) {
            syntax_error();
        }
        if (n_kw) {
            mp_raise_TypeError(MP_ERROR_TEXT("struct: no fields"));
        }
    } else {
        mp_obj_dict_t *d = MP_OBJ_TO_PTR(desc);
        // only for packed ROM tables..
        assert(d->map.alloc == d->map.used);
        assert(d->map.all_keys_are_qstrs);

        if (n_args > d->map.alloc) {
            mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("struct: index out of range"));
        }
        for (size_t i = 0; i < n_args; i++) {
            mp_store_attr(result, mp_obj_str_get_qstr(d->map.table[i].key), args[i]);
        }
        args += n_args;
        for (size_t i = 0; i < 2 * n_kw; i += 2) {
            qstr q = mp_obj_str_get_qstr(args[i]);
            for (size_t j = 0; j < n_args; j++) {
                if (mp_obj_str_get_qstr(d->map.table[j].key) == q) {
                    mp_raise_msg_varg(&mp_type_TypeError,
                        MP_ERROR_TEXT("function got multiple values for argument '%q'"), q);
                }
            }
            mp_store_attr(result, q, args[i + 1]);
        }
    }
    return result;
}

#define mp_obj_get_type_qstr(o_in) ((qstr)(mp_obj_get_type((o_in))->name))
void uctypes_struct_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_dict_t *d = 0;
    const char *typen = "unk";
    if (mp_obj_is_dict_or_ordereddict(self->desc)) {
        d = MP_OBJ_TO_PTR(self->desc);
        typen = "STRUCT";
    } else if (mp_obj_is_type(self->desc, &mp_type_tuple)) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->desc);
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
        uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
        switch (agg_type) {
            case PTR:
                typen = "PTR";
                break;
            case ARRAY:
                typen = "ARRAY";
                break;
        }
    } else {
        typen = "ERROR";
    }

    mp_printf(print, "<%q %s %p", (qstr)mp_obj_get_type_qstr(self_in), typen, struct_addr(self));
    if (kind == PRINT_REPR && d) {
        for (mp_uint_t i = 0; i < d->map.alloc; i++) {
            if (mp_map_slot_is_filled(&d->map, i)) {
                qstr k = mp_obj_str_get_qstr(d->map.table[i].key);
                mp_obj_t attr = uctypes_struct_attr_op(self_in, k, MP_OBJ_NULL);
                mp_printf(print, "\n    %q=%r", k, attr);
            }
        }
    }
    mp_printf(print, ">");
}

// Get size of scalar type descriptor
static inline mp_uint_t uctypes_struct_scalar_size(int val_type) {
    if (val_type == FLOAT32) {
        return 4;
    } else {
        return GET_SCALAR_SIZE(val_type & 7);
    }
}

// Get size of aggregate type descriptor
static mp_uint_t uctypes_struct_agg_size(mp_obj_tuple_t *t, int layout_type, mp_uint_t *max_field_size) {
    if (t->len == 0) {
        syntax_error();
    }

    mp_uint_t total_size = 0;

    mp_int_t offset_ = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
    mp_uint_t agg_type = GET_TYPE(offset_, AGG_TYPE_BITS);

    switch (agg_type) {
        case STRUCT:
            if (t->len != 2) {
                syntax_error();
            }
            return uctypes_struct_size(t->items[1], layout_type, max_field_size);
        case PTR:
            // Second field ignored, but should still be present for consistency.
            if (t->len != 2) {
                syntax_error();
            }
            if (sizeof(void *) > *max_field_size) {
                *max_field_size = sizeof(void *);
            }
            return sizeof(void *);
        case ARRAY: {
            mp_int_t arr_sz = MP_OBJ_SMALL_INT_VALUE(t->items[1]);
            uint val_type = GET_TYPE(arr_sz, VAL_TYPE_BITS);
            arr_sz &= VALUE_MASK(VAL_TYPE_BITS);
            mp_uint_t item_s;
            if (t->len == 2) {
                // Elements of array are scalar
                item_s = uctypes_struct_scalar_size(val_type);
                if (item_s > *max_field_size) {
                    *max_field_size = item_s;
                }
            } else if (t->len == 3) {
                // Elements of array are aggregates
                item_s = uctypes_struct_size(t->items[2], layout_type, max_field_size);
            } else {
                syntax_error();
            }

            return item_s * arr_sz;
        }
        default:
            syntax_error();
    }

    return total_size;
}

static mp_uint_t uctypes_struct_size(mp_obj_t desc_in, int layout_type, mp_uint_t *max_field_size) {
    if (is_struct_type(desc_in)) {
        mp_obj_ctypes_struct_type_t *struct_type = MP_OBJ_TO_PTR(desc_in);
        desc_in = struct_type->desc;
    }
    if (!mp_obj_is_dict_or_ordereddict(desc_in)) {
        if (mp_obj_is_type(desc_in, &mp_type_tuple)) {
            return uctypes_struct_agg_size((mp_obj_tuple_t *)MP_OBJ_TO_PTR(desc_in), layout_type, max_field_size);
        } else if (mp_obj_is_small_int(desc_in)) {
            // We allow sizeof on both type definitions and structures/structure fields,
            // but scalar structure field is lowered into native Python int, so all
            // type info is lost. So, we cannot say if it's scalar type description,
            // or such lowered scalar.
            mp_raise_TypeError(MP_ERROR_TEXT("can't unambiguously get sizeof scalar"));
        }
        syntax_error();
    }

    mp_obj_dict_t *d = MP_OBJ_TO_PTR(desc_in);
    mp_uint_t total_size = 0;

    for (mp_uint_t i = 0; i < d->map.alloc; i++) {
        if (mp_map_slot_is_filled(&d->map, i)) {
            mp_obj_t v = d->map.table[i].value;
            if (mp_obj_is_small_int(v)) {
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
                if (!mp_obj_is_type(v, &mp_type_tuple)) {
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

static mp_obj_t uctypes_struct_sizeof(size_t n_args, const mp_obj_t *args) {
    mp_obj_t obj_in = args[0];
    mp_uint_t max_field_size = 0;
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(obj_in, &bufinfo, MP_BUFFER_READ)) {
        if (n_args != 1) {
            mp_raise_TypeError(NULL);
        }
        return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
    }
    int layout_type = LAYOUT_NATIVE;
    // We can apply sizeof to structure definition (a dict) or a "struct type" type
    if (is_struct_type(obj_in)) {
        if (n_args != 1) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_ctypes_struct_type_t *obj = MP_OBJ_TO_PTR(obj_in);
        obj_in = obj->desc;
        layout_type = obj->struct_flags;
    } else {
        if (n_args == 2) {
            layout_type = mp_obj_get_int(args[1]);
        }
    }
    mp_uint_t size = uctypes_struct_size(obj_in, layout_type, &max_field_size);
    return MP_OBJ_NEW_SMALL_INT(size);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uctypes_struct_sizeof_obj, 1, 2, uctypes_struct_sizeof);

mp_obj_t uctypes_get_struct_desc(mp_obj_t arg) {
    if (is_struct_instance(arg)) {
        mp_obj_uctypes_struct_t *struct_ = MP_OBJ_TO_PTR(arg);
        return struct_->desc;
    }
    if (is_struct_type(arg)) {
        mp_obj_ctypes_struct_type_t *struct_type = MP_OBJ_TO_PTR(arg);
        return struct_type->desc;
    }
    return MP_OBJ_NULL;
}
static mp_obj_t uctypes_struct_desc(mp_obj_t arg) {
    mp_obj_t result = uctypes_get_struct_desc(arg);
    if (result == MP_OBJ_NULL) {
        mp_raise_TypeError(NULL);
    }
    return result;
}

MP_DEFINE_CONST_FUN_OBJ_1(uctypes_struct_desc_obj, uctypes_struct_desc);

static const char type2char[16] = {
    'B', 'b', 'H', 'h', 'I', 'i', 'Q', 'q',
    '-', '-', '-', '-', '-', '-', 'f', 'd'
};

static inline mp_obj_t get_unaligned(uint val_type, byte *p, int big_endian) {
    char struct_type = big_endian ? '>' : '<';
    return mp_binary_get_val(struct_type, type2char[val_type], p, &p);
}

static inline void set_unaligned(uint val_type, byte *p, int big_endian, mp_obj_t val) {
    char struct_type = big_endian ? '>' : '<';
    mp_binary_set_val(struct_type, type2char[val_type], val, p, &p);
}

static inline mp_uint_t get_aligned_basic(uint val_type, void *p) {
    switch (val_type) {
        case UINT8:
            return *(uint8_t *)p;
        case UINT16:
            return *(uint16_t *)p;
        case UINT32:
            return *(uint32_t *)p;
    }
    assert(0);
    return 0;
}

static inline void set_aligned_basic(uint val_type, void *p, mp_uint_t v) {
    switch (val_type) {
        case UINT8:
            *(uint8_t *)p = (uint8_t)v;
            return;
        case UINT16:
            *(uint16_t *)p = (uint16_t)v;
            return;
        case UINT32:
            *(uint32_t *)p = (uint32_t)v;
            return;
    }
    assert(0);
}

static mp_obj_t get_aligned(uint val_type, void *p, mp_int_t index) {
    switch (val_type) {
        case UINT8:
            return MP_OBJ_NEW_SMALL_INT(((uint8_t *)p)[index]);
        case INT8:
            return MP_OBJ_NEW_SMALL_INT(((int8_t *)p)[index]);
        case UINT16:
            return MP_OBJ_NEW_SMALL_INT(((uint16_t *)p)[index]);
        case INT16:
            return MP_OBJ_NEW_SMALL_INT(((int16_t *)p)[index]);
        case UINT32:
            return mp_obj_new_int_from_uint(((uint32_t *)p)[index]);
        case INT32:
            return mp_obj_new_int(((int32_t *)p)[index]);
        case UINT64:
            return mp_obj_new_int_from_ull(((uint64_t *)p)[index]);
        case INT64:
            return mp_obj_new_int_from_ll(((int64_t *)p)[index]);
        #if MICROPY_PY_BUILTINS_FLOAT
        case FLOAT32:
            return mp_obj_new_float_from_f(((float *)p)[index]);
        case FLOAT64:
            return mp_obj_new_float_from_d(((double *)p)[index]);
        #endif
        default:
            assert(0);
            return MP_OBJ_NULL;
    }
}

static void set_aligned(uint val_type, void *p, mp_int_t index, mp_obj_t val) {
    #if MICROPY_PY_BUILTINS_FLOAT
    if (val_type == FLOAT32 || val_type == FLOAT64) {
        if (val_type == FLOAT32) {
            ((float *)p)[index] = mp_obj_get_float_to_f(val);
        } else {
            ((double *)p)[index] = mp_obj_get_float_to_d(val);
        }
        return;
    }
    #endif
    mp_int_t v = mp_obj_get_int_truncated(val);
    switch (val_type) {
        case UINT8:
            ((uint8_t *)p)[index] = (uint8_t)v;
            return;
        case INT8:
            ((int8_t *)p)[index] = (int8_t)v;
            return;
        case UINT16:
            ((uint16_t *)p)[index] = (uint16_t)v;
            return;
        case INT16:
            ((int16_t *)p)[index] = (int16_t)v;
            return;
        case UINT32:
            ((uint32_t *)p)[index] = (uint32_t)v;
            return;
        case INT32:
            ((int32_t *)p)[index] = (int32_t)v;
            return;
        case INT64:
        case UINT64:
            if (sizeof(mp_int_t) == 8) {
                ((uint64_t *)p)[index] = (uint64_t)v;
            } else {
                // TODO: Doesn't offer atomic store semantics, but should at least try
                set_unaligned(val_type, (void *)&((uint64_t *)p)[index], MP_ENDIANNESS_BIG, val);
            }
            return;
        default:
            assert(0);
    }
}

static mp_obj_t uctypes_struct_attr_op(mp_obj_t self_in, qstr attr, mp_obj_t set_val) {
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);

    if (!mp_obj_is_dict_or_ordereddict(self->desc)) {
        mp_raise_TypeError(MP_ERROR_TEXT("struct: no fields"));
    }

    mp_obj_t deref = mp_obj_dict_get(self->desc, MP_OBJ_NEW_QSTR(attr));
    if (mp_obj_is_small_int(deref)) {
        mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(deref);
        mp_uint_t val_type = GET_TYPE(offset, VAL_TYPE_BITS);
        offset &= VALUE_MASK(VAL_TYPE_BITS);

        if (val_type <= INT64 || val_type == FLOAT32 || val_type == FLOAT64) {
            if (self->flags == LAYOUT_NATIVE) {
                if (set_val == MP_OBJ_NULL) {
                    return get_aligned(val_type, struct_addr(self) + offset, 0);
                } else {
                    set_aligned(val_type, struct_addr(self) + offset, 0, set_val);
                    return set_val; // just !MP_OBJ_NULL
                }
            } else {
                if (set_val == MP_OBJ_NULL) {
                    return get_unaligned(val_type, struct_addr(self) + offset, self->flags);
                } else {
                    set_unaligned(val_type, struct_addr(self) + offset, self->flags, set_val);
                    return set_val; // just !MP_OBJ_NULL
                }
            }
        } else if (val_type >= BFUINT8 && val_type <= BFINT32) {
            uint bit_offset = (offset >> OFFSET_BITS) & 31;
            uint bit_len = (offset >> LEN_BITS) & 31;
            offset &= (1 << OFFSET_BITS) - 1;
            mp_uint_t val;
            if (self->flags == LAYOUT_NATIVE) {
                val = get_aligned_basic(val_type & 6, struct_addr(self) + offset);
            } else {
                val = mp_binary_get_int(GET_SCALAR_SIZE(val_type & 7), val_type & 1, self->flags, struct_addr(self) + offset);
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
                    set_aligned_basic(val_type & 6, struct_addr(self) + offset, val);
                } else {
                    mp_binary_set_int(GET_SCALAR_SIZE(val_type & 7), self->flags == LAYOUT_BIG_ENDIAN,
                        struct_addr(self) + offset, val);
                }
                return set_val; // just !MP_OBJ_NULL
            }
        }

        assert(0);
        return MP_OBJ_NULL;
    }

    if (!mp_obj_is_type(deref, &mp_type_tuple)) {
        syntax_error();
    }

    if (set_val != MP_OBJ_NULL) {
        // Cannot assign to aggregate
        mp_raise_TypeError(MP_ERROR_TEXT("cannot assign to aggregate"));
    }

    mp_obj_tuple_t *sub = MP_OBJ_TO_PTR(deref);
    mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(sub->items[0]);
    mp_uint_t agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
    offset &= VALUE_MASK(AGG_TYPE_BITS);

    switch (agg_type) {
        case STRUCT: {
            mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)(uintptr_t)struct_addr(self) + offset), sub->items[1], mp_obj_new_int(self->flags) };
            return uctypes_struct_make_new(&uctypes_struct_type, MP_ARRAY_SIZE(args), 0, args);
        }
        case ARRAY: {
            mp_uint_t dummy;
            if (IS_SCALAR_ARRAY(sub) && IS_SCALAR_ARRAY_OF_BYTES(sub)) {
                return mp_obj_new_bytearray_by_ref(uctypes_struct_agg_size(sub, self->flags, &dummy), struct_addr(self) + offset);
            }
            // Fall thru to return uctypes struct object
            MP_FALLTHROUGH
        }
        case PTR: {
            mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)(uintptr_t)struct_addr(self) + offset), deref, mp_obj_new_int(self->flags) };
            return uctypes_struct_make_new(&uctypes_struct_type, MP_ARRAY_SIZE(args), 0, args);
        }
    }

    // Should be unreachable once all cases are handled
    return MP_OBJ_NULL;
}

void uctypes_struct_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
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

static bool is_all_slice(mp_obj_t index_in) {
    if (!mp_obj_is_type(index_in, &mp_type_slice)) {
        return false;
    }
    mp_obj_slice_t *index = MP_OBJ_TO_PTR(index_in);
    return index->start == mp_const_none
           && index->stop == mp_const_none
           && index->step == mp_const_none;
}

mp_obj_t uctypes_struct_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);

    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    } else {
        if (is_all_slice(index_in)) {
            mp_buffer_info_t src_buf;
            mp_buffer_info_t dst_buf;
            mp_obj_t result;
            if (value == MP_OBJ_SENTINEL) {
                // the all-slice is on the RHS, so return a copy
                mp_get_buffer_raise(self_in, &src_buf, MP_BUFFER_READ);
                result = mp_obj_new_bytearray(src_buf.len, src_buf.buf);
                mp_obj_t args1[] = {result, self->desc, mp_obj_new_int(self->flags)};
                return uctypes_struct_make_new(mp_obj_get_type(self_in), MP_ARRAY_SIZE(args1), 0, args1);
            } else {
                // the all-slice is on the LHS, so copy this object
                mp_get_buffer_raise(self_in, &dst_buf, MP_BUFFER_WRITE);
                mp_get_buffer_raise(value, &src_buf, MP_BUFFER_READ);

                if (dst_buf.len != src_buf.len) {
                    mp_raise_TypeError(MP_ERROR_TEXT("struct: object size mismatch"));
                }
                memcpy(dst_buf.buf, src_buf.buf, dst_buf.len);
                return value;
            }
        }
        // load / store
        if (!mp_obj_is_type(self->desc, &mp_type_tuple)) {
            mp_raise_TypeError(MP_ERROR_TEXT("struct: can't index"));
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
                mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("struct: index out of range"));
            }

            if (t->len == 2) {
                // array of scalars
                if (self->flags == LAYOUT_NATIVE) {
                    if (value == MP_OBJ_SENTINEL) {
                        return get_aligned(val_type, struct_addr(self), index);
                    } else {
                        set_aligned(val_type, struct_addr(self), index, value);
                        return value; // just !MP_OBJ_NULL
                    }
                } else {
                    byte *p = struct_addr(self) + uctypes_struct_scalar_size(val_type) * index;
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
                mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)(uintptr_t)struct_addr(self) + size * index), t->items[2], mp_obj_new_int(self->flags) };
                return uctypes_struct_make_new(&uctypes_struct_type, MP_ARRAY_SIZE(args), 0, args);
            } else {
                return MP_OBJ_NULL; // op not supported
            }

        } else if (agg_type == PTR) {
            byte *p = *(void **)struct_addr(self);
            if (mp_obj_is_small_int(t->items[1])) {
                uint val_type = GET_TYPE(MP_OBJ_SMALL_INT_VALUE(t->items[1]), VAL_TYPE_BITS);
                if (value == MP_OBJ_SENTINEL) {
                    return get_aligned(val_type, p, index);
                } else {
                    set_aligned(val_type, p, index, value);
                    return value; // just !MP_OBJ_NULL
                }
            } else {
                mp_uint_t dummy = 0;
                mp_uint_t size = uctypes_struct_size(t->items[1], self->flags, &dummy);
                mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)(uintptr_t)p + size * index), t->items[1], mp_obj_new_int(self->flags) };
                return uctypes_struct_make_new(&uctypes_struct_type, MP_ARRAY_SIZE(args), 0, args);
            }
        }

        assert(0);
        return MP_OBJ_NULL;
    }
}

mp_obj_t uctypes_struct_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_INT_MAYBE:
            if (mp_obj_is_type(self->desc, &mp_type_tuple)) {
                mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->desc);
                mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(t->items[0]);
                uint agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
                if (agg_type == PTR) {
                    byte *p = *(void **)struct_addr(self);
                    return mp_obj_new_int_from_uint((uintptr_t)p);
                }
            }
            MP_FALLTHROUGH

        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

mp_int_t uctypes_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_obj_uctypes_struct_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t max_field_size = 0;
    mp_uint_t size = uctypes_struct_size(self->desc, self->flags, &max_field_size);

    bufinfo->buf = struct_addr(self);
    bufinfo->len = size;
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    if (flags & MP_BUFFER_GET_BASE) {
        bufinfo->base = (void *)self->ptrbase;
    }

    return 0;
}

// addressof()
// Return address of object's data (applies to objects providing the buffer interface).
static mp_obj_t uctypes_struct_addressof(mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    return mp_obj_new_int_from_uint((uintptr_t)bufinfo.buf);
}
MP_DEFINE_CONST_FUN_OBJ_1(uctypes_struct_addressof_obj, uctypes_struct_addressof);

// bytearray_at()
// Capture memory at given address of given size as bytearray.
static mp_obj_t uctypes_struct_bytearray_at(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytearray_by_ref(mp_obj_int_get_truncated(size), (void *)(uintptr_t)mp_obj_int_get_truncated(ptr));
}
MP_DEFINE_CONST_FUN_OBJ_2(uctypes_struct_bytearray_at_obj, uctypes_struct_bytearray_at);

// bytes_at()
// Capture memory at given address of given size as bytes.
static mp_obj_t uctypes_struct_bytes_at(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytes((void *)(uintptr_t)mp_obj_int_get_truncated(ptr), mp_obj_int_get_truncated(size));
}
MP_DEFINE_CONST_FUN_OBJ_2(uctypes_struct_bytes_at_obj, uctypes_struct_bytes_at);

MP_DEFINE_CONST_OBJ_TYPE(
    uctypes_struct_type,
    MP_QSTR_struct,
    MP_TYPE_FLAG_NONE,
    make_new, uctypes_struct_make_new,
    print, uctypes_struct_print,
    attr, uctypes_struct_attr,
    subscr, uctypes_struct_subscr,
    unary_op, uctypes_struct_unary_op,
    buffer, uctypes_get_buffer
    );

static const mp_rom_map_elem_t mp_module_uctypes_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uctypes) },
    { MP_ROM_QSTR(MP_QSTR_struct), MP_ROM_PTR(&uctypes_struct_type) },
    { MP_ROM_QSTR(MP_QSTR_sizeof), MP_ROM_PTR(&uctypes_struct_sizeof_obj) },
    { MP_ROM_QSTR(MP_QSTR_addressof), MP_ROM_PTR(&uctypes_struct_addressof_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes_at), MP_ROM_PTR(&uctypes_struct_bytes_at_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytearray_at), MP_ROM_PTR(&uctypes_struct_bytearray_at_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_descr), MP_ROM_PTR(&uctypes_struct_desc_obj) },

    { MP_ROM_QSTR(MP_QSTR_NATIVE), MP_ROM_INT(LAYOUT_NATIVE) },
    { MP_ROM_QSTR(MP_QSTR_LITTLE_ENDIAN), MP_ROM_INT(LAYOUT_LITTLE_ENDIAN) },
    { MP_ROM_QSTR(MP_QSTR_BIG_ENDIAN), MP_ROM_INT(LAYOUT_BIG_ENDIAN) },

    { MP_ROM_QSTR(MP_QSTR_VOID), MP_ROM_INT(TYPE2SMALLINT(UINT8, VAL_TYPE_BITS)) },

    { MP_ROM_QSTR(MP_QSTR_UINT8), MP_ROM_INT(TYPE2SMALLINT(UINT8, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_INT8), MP_ROM_INT(TYPE2SMALLINT(INT8, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_UINT16), MP_ROM_INT(TYPE2SMALLINT(UINT16, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_INT16), MP_ROM_INT(TYPE2SMALLINT(INT16, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_UINT32), MP_ROM_INT(TYPE2SMALLINT(UINT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_INT32), MP_ROM_INT(TYPE2SMALLINT(INT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_UINT64), MP_ROM_INT(TYPE2SMALLINT(UINT64, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_INT64), MP_ROM_INT(TYPE2SMALLINT(INT64, VAL_TYPE_BITS)) },

    { MP_ROM_QSTR(MP_QSTR_BFUINT8), MP_ROM_INT(TYPE2SMALLINT(BFUINT8, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT8), MP_ROM_INT(TYPE2SMALLINT(BFINT8, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_BFUINT16), MP_ROM_INT(TYPE2SMALLINT(BFUINT16, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT16), MP_ROM_INT(TYPE2SMALLINT(BFINT16, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_BFUINT32), MP_ROM_INT(TYPE2SMALLINT(BFUINT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_BFINT32), MP_ROM_INT(TYPE2SMALLINT(BFINT32, VAL_TYPE_BITS)) },

    { MP_ROM_QSTR(MP_QSTR_BF_POS), MP_ROM_INT(OFFSET_BITS) },
    { MP_ROM_QSTR(MP_QSTR_BF_LEN), MP_ROM_INT(LEN_BITS) },

    #if MICROPY_PY_BUILTINS_FLOAT
    { MP_ROM_QSTR(MP_QSTR_FLOAT32), MP_ROM_INT(TYPE2SMALLINT(FLOAT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_FLOAT64), MP_ROM_INT(TYPE2SMALLINT(FLOAT64, VAL_TYPE_BITS)) },
    #endif

    #if MICROPY_PY_UCTYPES_NATIVE_C_TYPES
    // C native type aliases. These depend on GCC-compatible predefined
    // preprocessor macros.
    #if __SIZEOF_SHORT__ == 2
    { MP_ROM_QSTR(MP_QSTR_SHORT), MP_ROM_INT(TYPE2SMALLINT(INT16, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_USHORT), MP_ROM_INT(TYPE2SMALLINT(UINT16, VAL_TYPE_BITS)) },
    #endif
    #if __SIZEOF_INT__ == 4
    { MP_ROM_QSTR(MP_QSTR_INT), MP_ROM_INT(TYPE2SMALLINT(INT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_UINT), MP_ROM_INT(TYPE2SMALLINT(UINT32, VAL_TYPE_BITS)) },
    #endif
    #if __SIZEOF_LONG__ == 4
    { MP_ROM_QSTR(MP_QSTR_LONG), MP_ROM_INT(TYPE2SMALLINT(INT32, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_ULONG), MP_ROM_INT(TYPE2SMALLINT(UINT32, VAL_TYPE_BITS)) },
    #elif __SIZEOF_LONG__ == 8
    { MP_ROM_QSTR(MP_QSTR_LONG), MP_ROM_INT(TYPE2SMALLINT(INT64, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_ULONG), MP_ROM_INT(TYPE2SMALLINT(UINT64, VAL_TYPE_BITS)) },
    #endif
    #if __SIZEOF_LONG_LONG__ == 8
    { MP_ROM_QSTR(MP_QSTR_LONGLONG), MP_ROM_INT(TYPE2SMALLINT(INT64, VAL_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_ULONGLONG), MP_ROM_INT(TYPE2SMALLINT(UINT64, VAL_TYPE_BITS)) },
    #endif
    #endif // MICROPY_PY_UCTYPES_NATIVE_C_TYPES

    { MP_ROM_QSTR(MP_QSTR_PTR), MP_ROM_INT(TYPE2SMALLINT(PTR, AGG_TYPE_BITS)) },
    { MP_ROM_QSTR(MP_QSTR_ARRAY), MP_ROM_INT(TYPE2SMALLINT(ARRAY, AGG_TYPE_BITS)) },
};
static MP_DEFINE_CONST_DICT(mp_module_uctypes_globals, mp_module_uctypes_globals_table);

const mp_obj_module_t mp_module_uctypes = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_uctypes_globals,
};

// uctypes is not a Python standard library module (hence "uctypes"
// not "ctypes") and therefore shouldn't be extensible.
MP_REGISTER_MODULE(MP_QSTR_uctypes, mp_module_uctypes);

#endif
