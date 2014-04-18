#include <stdint.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "binary.h"

// Helpers to work with binary-encoded data

int mp_binary_get_size(char typecode) {
    // This assumes that unsigned and signed types are of the same type,
    // which is invariant for [u]intN_t.
    switch (typecode) {
        case BYTEARRAY_TYPECODE:
        case 'b':
        case 'B':
            return sizeof(int8_t);
        case 'h':
        case 'H':
            return sizeof(int16_t);
        case 'i':
        case 'I':
            return sizeof(int32_t);
        case 'l':
        case 'L':
            return sizeof(int32_t);
        case 'q':
        case 'Q':
            return sizeof(long long);
#if MICROPY_ENABLE_FLOAT
        case 'f':
            return sizeof(float);
        case 'd':
            return sizeof(double);
#endif
    }
    return -1;
}

mp_obj_t mp_binary_get_val_array(char typecode, void *p, int index) {
    machine_int_t val = 0;
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
#if MICROPY_ENABLE_FLOAT
        case 'f':
            return mp_obj_new_float(((float*)p)[index]);
        case 'd':
            return mp_obj_new_float(((double*)p)[index]);
#endif
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}

#define is_signed(typecode) (typecode > 'Z')
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte **ptr) {
    byte *p = *ptr;
    uint size = 0;
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
            uint align = 0;
            switch (val_type) {
                case 'b': case 'B':
                    align = size = 1; break;
                case 'h': case 'H':
                    align = size = sizeof(short); break;
                case 'i': case 'I':
                    align = size = sizeof(int); break;
                case 'l': case 'L':
                    align = size = sizeof(long); break;
            }
            // Make pointer aligned
            p = (byte*)(((machine_uint_t)p + align - 1) & ~(align - 1));
            #if MP_ENDIANNESS_LITTLE
            struct_type = '<';
            #else
            struct_type = '>';
            #endif
            break;
        }
    }

    int delta;
    if (struct_type == '<') {
        delta = -1;
        p += size - 1;
    } else {
        delta = 1;
    }

    machine_int_t val = 0;
    if (is_signed(val_type) && *p & 0x80) {
        val = -1;
    }
    for (uint i = 0; i < size; i++) {
        val <<= 8;
        val |= *p;
        p += delta;
    }

    *ptr += size;
    if (is_signed(val_type)) {
        return mp_obj_new_int(val);
    } else {
        return mp_obj_new_int_from_uint(val);
    }
}

void mp_binary_set_val_array(char typecode, void *p, int index, mp_obj_t val_in) {
    switch (typecode) {
#if MICROPY_ENABLE_FLOAT
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

void mp_binary_set_val_array_from_int(char typecode, void *p, int index, machine_int_t val) {
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
#if MICROPY_ENABLE_FLOAT
        case 'f':
            ((float*)p)[index] = val;
            break;
        case 'd':
            ((double*)p)[index] = val;
            break;
#endif
    }
}
