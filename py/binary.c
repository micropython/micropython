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

mp_obj_t mp_binary_get_val(char typecode, void *p, int index) {
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
mp_obj_t mp_binary_get_val_unaligned(char typecode, byte **ptr) {
    char type = '<';
    byte *p = *ptr;
    uint size = 0, align = 0;
    switch (type) {
        case '<': case '>':
            switch (typecode) {
                case 'b': case 'B':
                    size = 1; break;
                case 'h': case 'H':
                    size = 2; break;
                case 'i': case 'I':
                    size = 4; break;
            }
            break;
    }

    int delta;
    if (type == '<') {
        delta = -1;
        p += size - 1;
    } else {
        delta = 1;
    }

    machine_int_t val = 0;
    if (is_signed(typecode) && *p & 0x80) {
        val = -1;
    }
    for (uint i = 0; i < size; i++) {
        val <<= 8;
        val |= *p;
        p += delta;
    }

    *ptr += size + align;
    if (is_signed(typecode)) {
        return mp_obj_new_int(val);
    } else {
        return mp_obj_new_int_from_uint(val);
    }
}

void mp_binary_set_val(char typecode, void *p, int index, mp_obj_t val_in) {
    machine_int_t val = 0;
    if (MP_OBJ_IS_INT(val_in)) {
        val = mp_obj_int_get(val_in);
    }

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
            ((float*)p)[index] = mp_obj_float_get(val_in);
            break;
        case 'd':
            ((double*)p)[index] = mp_obj_float_get(val_in);
            break;
#endif
    }
}
