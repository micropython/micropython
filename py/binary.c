/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2017 Paul Sokolovsky
 * Copyright (c) 2014-2019 Damien P. George
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

#include "py/binary.h"
#include "py/smallint.h"
#include "py/objint.h"
#include "py/runtime.h"

// Helpers to work with binary-encoded data

#ifndef alignof
#define alignof(type) offsetof(struct { char c; type t; }, t)
#endif

size_t mp_binary_get_size(char struct_type, char val_type, mp_uint_t *palign) {
    size_t size = 0;
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
                case 'f':
                    size = sizeof(float); break;
                case 'd':
                    size = sizeof(double); break;
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
                case 'f':
                    align = alignof(float);
                    size = sizeof(float); break;
                case 'd':
                    align = alignof(double);
                    size = sizeof(double); break;
            }
        }
    }

    if (size == 0) {
        mp_raise_ValueError("bad typecode");
    }

    if (palign != NULL) {
        *palign = align;
    }
    return size;
}

mp_obj_t mp_binary_get_val_array(char typecode, void *p, mp_uint_t index) {
    mp_int_t val = 0;
    switch (typecode) {
        case 'b':
            val = ((signed char*)p)[index];
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            val = ((unsigned char*)p)[index];
            break;
        case 'h':
            val = ((short*)p)[index];
            break;
        case 'H':
            val = ((unsigned short*)p)[index];
            break;
        case 'i':
            return mp_obj_new_int(((int*)p)[index]);
        case 'I':
            return mp_obj_new_int_from_uint(((unsigned int*)p)[index]);
        case 'l':
            return mp_obj_new_int(((long*)p)[index]);
        case 'L':
            return mp_obj_new_int_from_uint(((unsigned long*)p)[index]);
        #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
        case 'q':
            return mp_obj_new_int_from_ll(((long long*)p)[index]);
        case 'Q':
            return mp_obj_new_int_from_ull(((unsigned long long*)p)[index]);
        #endif
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            return mp_obj_new_float(((float*)p)[index]);
        case 'd':
            return mp_obj_new_float(((double*)p)[index]);
#endif
        // Extension to CPython: array of objects
        case 'O':
            return ((mp_obj_t*)p)[index];
        // Extension to CPython: array of pointers
        case 'P':
            return mp_obj_new_int((mp_int_t)(uintptr_t)((void**)p)[index]);
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}

// The long long type is guaranteed to hold at least 64 bits, and size is at
// most 8 (for q and Q), so we will always be able to parse the given data
// and fit it into a long long.
long long mp_binary_get_int(mp_uint_t size, bool is_signed, bool big_endian, const byte *src) {
    int delta;
    if (!big_endian) {
        delta = -1;
        src += size - 1;
    } else {
        delta = 1;
    }

    long long val = 0;
    if (is_signed && *src & 0x80) {
        val = -1;
    }
    for (uint i = 0; i < size; i++) {
        val <<= 8;
        val |= *src;
        src += delta;
    }

    return val;
}

#define is_signed(typecode) (typecode > 'Z')
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte **ptr) {
    byte *p = *ptr;
    mp_uint_t align;

    size_t size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Make pointer aligned
        p = (byte*)MP_ALIGN(p, (size_t)align);
        #if MP_ENDIANNESS_LITTLE
        struct_type = '<';
        #else
        struct_type = '>';
        #endif
    }
    *ptr = p + size;

    long long val = mp_binary_get_int(size, is_signed(val_type), (struct_type == '>'), p);

    if (val_type == 'O') {
        return (mp_obj_t)(mp_uint_t)val;
    } else if (val_type == 'S') {
        const char *s_val = (const char*)(uintptr_t)(mp_uint_t)val;
        return mp_obj_new_str(s_val, strlen(s_val));
#if MICROPY_PY_BUILTINS_FLOAT
    } else if (val_type == 'f') {
        union { uint32_t i; float f; } fpu = {val};
        return mp_obj_new_float(fpu.f);
    } else if (val_type == 'd') {
        union { uint64_t i; double f; } fpu = {val};
        return mp_obj_new_float(fpu.f);
#endif
    } else if (is_signed(val_type)) {
        if ((long long)MP_SMALL_INT_MIN <= val && val <= (long long)MP_SMALL_INT_MAX) {
            return mp_obj_new_int((mp_int_t)val);
        } else {
            return mp_obj_new_int_from_ll(val);
        }
    } else {
        if ((unsigned long long)val <= (unsigned long long)MP_SMALL_INT_MAX) {
            return mp_obj_new_int_from_uint((mp_uint_t)val);
        } else {
            return mp_obj_new_int_from_ull(val);
        }
    }
}

void mp_binary_set_int(mp_uint_t val_sz, bool big_endian, byte *dest, mp_uint_t val) {
    if (MP_ENDIANNESS_LITTLE && !big_endian) {
        memcpy(dest, &val, val_sz);
    } else if (MP_ENDIANNESS_BIG && big_endian) {
        // only copy the least-significant val_sz bytes
        memcpy(dest, (byte*)&val + sizeof(mp_uint_t) - val_sz, val_sz);
    } else {
        const byte *src;
        if (MP_ENDIANNESS_LITTLE) {
            src = (const byte*)&val + val_sz;
        } else {
            src = (const byte*)&val + sizeof(mp_uint_t);
        }
        while (val_sz--) {
            *dest++ = *--src;
        }
    }
}

void mp_binary_set_val(char struct_type, char val_type, mp_obj_t val_in, byte **ptr) {
    byte *p = *ptr;
    mp_uint_t align;

    size_t size = mp_binary_get_size(struct_type, val_type, &align);
    if (struct_type == '@') {
        // Make pointer aligned
        p = (byte*)MP_ALIGN(p, (size_t)align);
        if (MP_ENDIANNESS_LITTLE) {
            struct_type = '<';
        } else {
            struct_type = '>';
        }
    }
    *ptr = p + size;

    mp_uint_t val;
    switch (val_type) {
        case 'O':
            val = (mp_uint_t)val_in;
            break;
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f': {
            union { uint32_t i; float f; } fp_sp;
            fp_sp.f = mp_obj_get_float(val_in);
            val = fp_sp.i;
            break;
        }
        case 'd': {
            union { uint64_t i64; uint32_t i32[2]; double f; } fp_dp;
            fp_dp.f = mp_obj_get_float(val_in);
            if (BYTES_PER_WORD == 8) {
                val = fp_dp.i64;
            } else {
                int be = struct_type == '>';
                mp_binary_set_int(sizeof(uint32_t), be, p, fp_dp.i32[MP_ENDIANNESS_BIG ^ be]);
                p += sizeof(uint32_t);
                val = fp_dp.i32[MP_ENDIANNESS_LITTLE ^ be];
            }
            break;
        }
#endif
        default:
            #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
            if (mp_obj_is_type(val_in, &mp_type_int)) {
                mp_obj_int_to_bytes_impl(val_in, struct_type == '>', size, p);
                return;
            } else
            #endif
            {
                val = mp_obj_get_int(val_in);
                // zero/sign extend if needed
                if (BYTES_PER_WORD < 8 && size > sizeof(val)) {
                    int c = (is_signed(val_type) && (mp_int_t)val < 0) ? 0xff : 0x00;
                    memset(p, c, size);
                    if (struct_type == '>') {
                        p += size - sizeof(val);
                    }
                }
            }
    }

    mp_binary_set_int(MIN((size_t)size, sizeof(val)), struct_type == '>', p, val);
}

void mp_binary_set_val_array(char typecode, void *p, mp_uint_t index, mp_obj_t val_in) {
    switch (typecode) {
#if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            ((float*)p)[index] = mp_obj_get_float(val_in);
            break;
        case 'd':
            ((double*)p)[index] = mp_obj_get_float(val_in);
            break;
#endif
        // Extension to CPython: array of objects
        case 'O':
            ((mp_obj_t*)p)[index] = val_in;
            break;
        default:
            #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
            if (mp_obj_is_type(val_in, &mp_type_int)) {
                size_t size = mp_binary_get_size('@', typecode, NULL);
                mp_obj_int_to_bytes_impl(val_in, MP_ENDIANNESS_BIG,
                    size, (uint8_t*)p + index * size);
                return;
            }
            #endif
            mp_binary_set_val_array_from_int(typecode, p, index, mp_obj_get_int(val_in));
    }
}

void mp_binary_set_val_array_from_int(char typecode, void *p, mp_uint_t index, mp_int_t val) {
    switch (typecode) {
        case 'b':
            ((signed char*)p)[index] = val;
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            ((unsigned char*)p)[index] = val;
            break;
        case 'h':
            ((short*)p)[index] = val;
            break;
        case 'H':
            ((unsigned short*)p)[index] = val;
            break;
        case 'i':
            ((int*)p)[index] = val;
            break;
        case 'I':
            ((unsigned int*)p)[index] = val;
            break;
        case 'l':
            ((long*)p)[index] = val;
            break;
        case 'L':
            ((unsigned long*)p)[index] = val;
            break;
        #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
        case 'q':
            ((long long*)p)[index] = val;
            break;
        case 'Q':
            ((unsigned long long*)p)[index] = val;
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
        // Extension to CPython: array of pointers
        case 'P':
            ((void**)p)[index] = (void*)(uintptr_t)val;
            break;
    }
}
