/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#include "py/obj.h"
#include "py/objarray.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"

#if MICROPY_VFS_ROM_IOCTL

#if MICROPY_HW_ROMFS_ENABLE_PART0 && !defined(MICROPY_HW_ROMFS_PART0_START)
#define MICROPY_HW_ROMFS_PART0_START (uintptr_t)(&_micropy_hw_romfs_part0_start)
#define MICROPY_HW_ROMFS_PART0_SIZE (uintptr_t)(&_micropy_hw_romfs_part0_size)
extern uint8_t _micropy_hw_romfs_part0_start;
extern uint8_t _micropy_hw_romfs_part0_size;
#endif

#if MICROPY_HW_ROMFS_ENABLE_PART1 && !defined(MICROPY_HW_ROMFS_PART1_START)
#define MICROPY_HW_ROMFS_PART1_START (uintptr_t)(&_micropy_hw_romfs_part1_start)
#define MICROPY_HW_ROMFS_PART1_SIZE (uintptr_t)(&_micropy_hw_romfs_part1_size)
extern uint8_t _micropy_hw_romfs_part1_start;
extern uint8_t _micropy_hw_romfs_part1_size;
#endif

#define ROMFS_MEMORYVIEW(base, size) {{&mp_type_memoryview}, 'B', 0, (size), (void *)(base)}

static const mp_obj_array_t romfs_obj_table[] = {
    #if MICROPY_HW_ROMFS_ENABLE_PART0
    ROMFS_MEMORYVIEW(MICROPY_HW_ROMFS_PART0_START, MICROPY_HW_ROMFS_PART0_SIZE),
    #endif
    #if MICROPY_HW_ROMFS_ENABLE_PART1
    ROMFS_MEMORYVIEW(MICROPY_HW_ROMFS_PART1_START, MICROPY_HW_ROMFS_PART1_SIZE),
    #endif
};

mp_obj_t mp_vfs_rom_ioctl(size_t n_args, const mp_obj_t *args) {
    mp_int_t cmd = mp_obj_get_int(args[0]);
    if (cmd == MP_VFS_ROM_IOCTL_GET_NUMBER_OF_SEGMENTS) {
        return MP_OBJ_NEW_SMALL_INT(MP_ARRAY_SIZE(romfs_obj_table));
    }

    if (n_args < 2) {
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    mp_int_t romfs_id = mp_obj_get_int(args[1]);
    if (!(0 <= romfs_id && romfs_id < MP_ARRAY_SIZE(romfs_obj_table))) {
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    if (cmd == MP_VFS_ROM_IOCTL_GET_SEGMENT) {
        // Return the ROMFS memoryview object.
        const mp_obj_array_t *romfs_obj = &romfs_obj_table[romfs_id];
        return MP_OBJ_FROM_PTR(romfs_obj);
    }

    return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
}
#endif // MICROPY_VFS_ROM_IOCTL
