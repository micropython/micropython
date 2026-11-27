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

#include "py/binary.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "py/objarray.h"
#include "extmod/modmachine.h"
#include <string.h>

#if MICROPY_PY_MACHINE_MEMX

// If you wish to override the functions for mapping the machine_mem read/write
// address, then add a #define for MICROPY_MACHINE_MEM_GET_READ_ADDR and/or
// MICROPY_MACHINE_MEM_GET_WRITE_ADDR in your mpconfigport.h. Since the
// prototypes are identical, it is allowable for both of the macros to evaluate
// the to same function.
//
// It is expected that the modmachine.c file for a given port will provide the
// implementations, if the default implementation isn't used.

#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR) || !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
static uintptr_t machine_mem_get_addr(uintptr_t addr, uint align) {
    if ((addr & (align - 1)) != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("address %08x is not aligned to %d bytes"), addr, align);
    }
    return addr;
}
#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR)
#define MICROPY_MACHINE_MEM_GET_READ_ADDR machine_mem_get_addr
#endif
#if !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
#define MICROPY_MACHINE_MEM_GET_WRITE_ADDR machine_mem_get_addr
#endif
#endif

static void machine_mem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<%u-bit memory>", 8 * self->elem_size);
}

#if MICROPY_PY_MACHINE_MEMX_SLICE && MICROPY_PY_BUILTINS_SLICE
static inline int machine_mem_typecode(machine_mem_obj_t *self) {
    switch (self->elem_size) {
        case 1:
            return 'B';
        case 2:
            return 'H';
        default:
            return 'I';
    }
}
#endif

static mp_obj_t machine_mem_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    #if MICROPY_PY_MACHINE_MEMX_SLICE && MICROPY_PY_BUILTINS_SLICE
    } else if (mp_obj_is_type(index, &mp_type_slice)) {
        mp_bound_slice_t slice;
        if (!mp_seq_get_fast_slice_indexes(INT32_MAX, index, &slice)) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
        }
        if (value == MP_OBJ_SENTINEL) {
            // load
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            return mp_obj_new_memoryview(
                machine_mem_typecode(self) | MP_OBJ_ARRAY_TYPECODE_FLAG_RW,
                (slice.stop - slice.start) / self->elem_size,
                (void *)MICROPY_MACHINE_MEM_GET_READ_ADDR((uintptr_t)slice.start, self->elem_size)
                );
            #else
            mp_raise_NotImplementedError(MP_ERROR_TEXT("memoryviews are not supported"));
            #endif
        } else {
            // store
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(value, &bufinfo, MP_BUFFER_READ);
            if (
                mp_binary_get_size('@', bufinfo.typecode, NULL) != self->elem_size
                ||
                bufinfo.len != (size_t)(slice.stop - slice.start)
                ) {
                mp_raise_ValueError(MP_ERROR_TEXT("lhs and rhs should be compatible"));
            }
            memcpy(
                (void *)MICROPY_MACHINE_MEM_GET_READ_ADDR((uintptr_t)slice.start, self->elem_size),
                bufinfo.buf,
                bufinfo.len
                );
            return mp_const_none;
        }
    #endif
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        uintptr_t addr = MICROPY_MACHINE_MEM_GET_READ_ADDR(mp_obj_get_int_truncated(index), self->elem_size);
        uint32_t val;
        switch (self->elem_size) {
            case 1:
                val = (*(uint8_t *)addr);
                break;
            case 2:
                val = (*(uint16_t *)addr);
                break;
            default:
                val = (*(uint32_t *)addr);
                break;
        }
        return mp_obj_new_int(val);
    } else {
        // store
        uintptr_t addr = MICROPY_MACHINE_MEM_GET_WRITE_ADDR(mp_obj_get_int_truncated(index), self->elem_size);
        uint32_t val = mp_obj_get_int_truncated(value);
        switch (self->elem_size) {
            case 1:
                (*(uint8_t *)addr) = val;
                break;
            case 2:
                (*(uint16_t *)addr) = val;
                break;
            default:
                (*(uint32_t *)addr) = val;
                break;
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_mem_type,
    MP_QSTR_mem,
    MP_TYPE_FLAG_NONE,
    print, machine_mem_print,
    subscr, machine_mem_subscr
    );

const machine_mem_obj_t machine_mem8_obj = {{&machine_mem_type}, 1};
const machine_mem_obj_t machine_mem16_obj = {{&machine_mem_type}, 2};
const machine_mem_obj_t machine_mem32_obj = {{&machine_mem_type}, 4};

#endif // MICROPY_PY_MACHINE_MEMX

#if MICROPY_PY_MACHINE_MEM_BACKUP

#if !MICROPY_PY_BUILTINS_MEMORYVIEW
#error "machine.mem_backup requires MICROPY_PY_BUILTINS_MEMORYVIEW"
#endif

// Convenience initialiser for a read-write memoryview entry in the
// machine_mem_backup_regions[] table provided by each port.  typecode is the
// base array typecode char ('B' for uint8, 'I' for uint32).
#define BACKUP_MV(typecode, len, ptr) \
    {{&mp_type_memoryview}, (typecode) | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, 0, (len), (ptr)}

// The port provides machine_mem_backup_regions[] in this file.
#include MICROPY_PY_MACHINE_MEM_BACKUP_INCLUDEFILE

// region=0 (default) returns region 0, region=-1 returns a tuple of all regions.
static mp_obj_t machine_mem_backup(size_t n_args, const mp_obj_t *args) {
    mp_int_t r = n_args ? mp_obj_get_int(args[0]) : 0;
    if (r == -1) {
        size_t n = MP_ARRAY_SIZE(machine_mem_backup_regions);
        mp_obj_tuple_t *tup = MP_OBJ_TO_PTR(mp_obj_new_tuple(n, NULL));
        for (size_t i = 0; i < n; i++) {
            tup->items[i] = MP_OBJ_FROM_PTR(&machine_mem_backup_regions[i]);
        }
        return MP_OBJ_FROM_PTR(tup);
    }
    if (r < 0 || (size_t)r >= MP_ARRAY_SIZE(machine_mem_backup_regions)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid region"));
    }
    return MP_OBJ_FROM_PTR(&machine_mem_backup_regions[r]);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_mem_backup_fun_obj, 0, 1, machine_mem_backup);

#endif // MICROPY_PY_MACHINE_MEM_BACKUP
