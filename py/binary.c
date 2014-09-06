/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "binary.h"

// Helpers to work with binary-encoded data

#ifndef alignof
#define alignof(type) offsetof(struct { char c; type t; }, t)
#endif

int mp_binary_get_size(char struct_type, char val_type, mp_uint_t *palign) {
    int size = 0;
    int align = 1;
    switch (struct_type) {
        case '<': case '>':
            switch (val_type) {
                case 'b': case 'B':
                    size = 1; break;
                case 'h': case 'H':
                    size = 2; break;
                case 'i': case 'I':
                    size = 4; break;
                case 'l': case 'L':
                    size = 4; break;
                case 'q': case 'Q':
                    size = 8; break;
                case 'P': case 'O': case 'S':
                    size = sizeof(void*); break;
            }
            break;
        case '@': {
            // TODO:
            // The simplest heuristic for alignment is to align by value
            // size, but that doesn't work for "bigger than int" types,
            // for example, long long may very well have long alignment
            // So, we introduce separate alignment handling, but having
            // formal support for that is different from actually supporting
            // particular (or any) ABI.
            switch (val_type) {
                case BYTEARRAY_TYPECODE:
                case 'b': case 'B':
                    align = size = 1; break;
                case 'h': case 'H':
                    align = alignof(short);
                    size = sizeof(short); break;
                case 'i': case 'I':
                    align = alignof(int);
                    size = sizeof(int); break;
                case 'l': case 'L':
                    align = alignof(long);
                    size = sizeof(long); break;
                case 'q': case 'Q':
                    align = alignof(long long);
                    size = sizeof(long long); break;
                case 'P': case 'O': case 'S':
                    align = alignof(void*);
                    size = sizeof(void*); break;
            }
        }
    }
    if (palign != NULL) {
        *palign = align;
    }
    return size;
}

mp_obj_t mp_binary_get_val_array(char typecode, void *p, int index) {
    mp_int_t val = 0;
    switch (typecode) {
        case 'b':
            val = ((int8_t*)p)[index];
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            val = ((uint8_t*)p)[index];
            break;
        case 'h':
            val = ((int16_t*)p)[index];
            break;
        case 'H':
            val = ((uint16_t*)p)[index];
            break;
        case 'i':
        case 'l':
            return mp_obj_new_int(((int32_t*)p)[index]);
        case 'I':
        case 'L':
            return mp_obj_new_int_from_uint(((uint32_t*)p)[index]);
#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
        case 'q':
        case 'Q':
            // TODO: Explode API more to cover signedness
            return mp_obj_new_int_from_ll(((long long*)p)[index]);
#endif
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            return mp_obj_new_float(((float*)p)[index]);
        case 'd':
            return mp_obj_new_float(((double*)p)[index]);
#endif
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}

mp_int_t mp_binary_get_int(mp_uint_t size, bool is_signed, bool big_endian, byte *p) {
    int delta;
    if (!big_endian) {
        delta = -1;
        p += size - 1;
    } else {
        delta = 1;
    }

    mp_int_t val = 0;
    if (is_signed && *p & 0x80) {
        val = -1;
    }
    for (uint i = 0; i < size; i++) {
        val <<= 8;
        val |= *p;
        p += delta;
    }

    return val;
}

#define is_signed(typecode) (typecode > 'Z')
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte **ptr) {
    byte *p = *ptr;
    mp_uint_t align;

    int size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Make pointer aligned
        p = (byte*)(((mp_uint_t)p + align - 1) & ~((mp_uint_t)align - 1));
        #if MP_ENDIANNESS_LITTLE
        struct_type = '<';
        #else
        struct_type = '>';
        #endif
    }
    *ptr = p + size;

    mp_int_t val = mp_binary_get_int(size, is_signed(val_type), (struct_type == '>'), p);

    if (val_type == 'O') {
        return (mp_obj_t)val;
    } else if (val_type == 'S') {
        return mp_obj_new_str((char*)val, strlen((char*)val), false);
    } else if (is_signed(val_type)) {
        return mp_obj_new_int(val);
    } else {
        return mp_obj_new_int_from_uint(val);
    }
}

void mp_binary_set_int(mp_uint_t val_sz, bool big_endian, byte *p, byte *val_ptr) {
    int in_delta, out_delta;
    if (big_endian) {
        in_delta = -1;
        out_delta = 1;
        val_ptr += val_sz - 1;
    } else {
        in_delta = out_delta = 1;
    }

    for (uint i = val_sz; i > 0; i--) {
        *p = *val_ptr;
        p += out_delta;
        val_ptr += in_delta;
    }
}

void mp_binary_set_val(char struct_type, char val_type, mp_obj_t val_in, byte **ptr) {
    byte *p = *ptr;
    mp_uint_t align;

    int size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Make pointer aligned
        p = (byte*)(((mp_uint_t)p + align - 1) & ~((mp_uint_t)align - 1));
        #if MP_ENDIANNESS_LITTLE
        struct_type = '<';
        #else
        struct_type = '>';
        #endif
    }
    *ptr = p + size;

#if MP_ENDIANNESS_BIG
#error Not implemented
#endif
    mp_int_t val;
    byte *in = (byte*)&val;
    switch (val_type) {
        case 'O':
            in = (byte*)&val_in;
            break;
        default:
            val = mp_obj_get_int(val_in);
    }

    mp_binary_set_int(MIN(size, sizeof(val)), struct_type == '>', p, in);
}

void mp_binary_set_val_array(char typecode, void *p, int index, mp_obj_t val_in) {
    switch (typecode) {
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            ((float*)p)[index] = mp_obj_float_get(val_in);
            break;
        case 'd':
            ((double*)p)[index] = mp_obj_float_get(val_in);
            break;
#endif
        default:
            mp_binary_set_val_array_from_int(typecode, p, index, mp_obj_get_int(val_in));
    }
}

void mp_binary_set_val_array_from_int(char typecode, void *p, int index, mp_int_t val) {
    switch (typecode) {
        case 'b':
            ((int8_t*)p)[index] = val;
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            val = ((uint8_t*)p)[index] = val;
            break;
        case 'h':
            val = ((int16_t*)p)[index] = val;
            break;
        case 'H':
            val = ((uint16_t*)p)[index] = val;
            break;
        case 'i':
        case 'l':
            ((int32_t*)p)[index] = val;
            break;
        case 'I':
        case 'L':
            ((uint32_t*)p)[index] = val;
            break;
#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
        case 'q':
        case 'Q':
            assert(0);
            ((long long*)p)[index] = val;
            break;
#endif
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            ((float*)p)[index] = val;
            break;
        case 'd':
            ((double*)p)[index] = val;
            break;
#endif
    }
}
