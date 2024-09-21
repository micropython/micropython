/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modrp2.h"
#include "hardware/flash.h"
#include "pico/binary_info.h"

#define BLOCK_SIZE_BYTES (FLASH_SECTOR_SIZE)

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (1408 * 1024)
#endif
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % 4096 == 0, "Flash storage size must be a multiple of 4K");

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (PICO_FLASH_SIZE_BYTES - MICROPY_HW_FLASH_STORAGE_BYTES)
#endif

static_assert(MICROPY_HW_FLASH_STORAGE_BYTES <= PICO_FLASH_SIZE_BYTES, "MICROPY_HW_FLASH_STORAGE_BYTES too big");
static_assert(MICROPY_HW_FLASH_STORAGE_BASE + MICROPY_HW_FLASH_STORAGE_BYTES <= PICO_FLASH_SIZE_BYTES, "MICROPY_HW_FLASH_STORAGE_BYTES too big");

typedef struct _rp2_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} rp2_flash_obj_t;

static rp2_flash_obj_t rp2_flash_obj = {
    .base = { &rp2_flash_type },
    .flash_base = MICROPY_HW_FLASH_STORAGE_BASE,
    .flash_size = MICROPY_HW_FLASH_STORAGE_BYTES,
};

// Tag the flash drive in the binary as readable/writable (but not reformatable)
bi_decl(bi_block_device(
    BINARY_INFO_TAG_MICROPYTHON,
    "MicroPython",
    XIP_BASE + MICROPY_HW_FLASH_STORAGE_BASE,
    MICROPY_HW_FLASH_STORAGE_BYTES,
    NULL,
    BINARY_INFO_BLOCK_DEV_FLAG_READ |
    BINARY_INFO_BLOCK_DEV_FLAG_WRITE |
    BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));

// Flash erase and write must run with interrupts disabled and the other core suspended,
// because the XIP bit gets disabled.
static uint32_t begin_critical_flash_section(void) {
    if (multicore_lockout_victim_is_initialized(1 - get_core_num())) {
        multicore_lockout_start_blocking();
    }
    return save_and_disable_interrupts();
}

static void end_critical_flash_section(uint32_t state) {
    restore_interrupts(state);
    if (multicore_lockout_victim_is_initialized(1 - get_core_num())) {
        multicore_lockout_end_blocking();
    }
}

static mp_obj_t rp2_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        #ifndef NDEBUG
        extern char __flash_binary_end;
        assert((uintptr_t)&__flash_binary_end - XIP_BASE <= MICROPY_HW_FLASH_STORAGE_BASE);
        #endif

        // Default singleton object that accesses entire flash
        return MP_OBJ_FROM_PTR(&rp2_flash_obj);
    }

    rp2_flash_obj_t *self = mp_obj_malloc(rp2_flash_obj_t, &rp2_flash_type);

    mp_int_t start = args[ARG_start].u_int;
    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    self->flash_base = MICROPY_HW_FLASH_STORAGE_BASE + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t rp2_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(bufinfo.buf, (void *)(XIP_BASE + self->flash_base + offset), bufinfo.len);
    // mp_event_handle_nowait() is called here to avoid a fail in registering
    // USB at boot time, if the board is busy loading files or scanning the file
    // system. mp_event_handle_nowait() will call the TinyUSB task if needed.
    mp_event_handle_nowait();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_flash_readblocks_obj, 3, 4, rp2_flash_readblocks);

static mp_obj_t rp2_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        mp_uint_t atomic_state = begin_critical_flash_section();
        flash_range_erase(self->flash_base + offset, bufinfo.len);
        end_critical_flash_section(atomic_state);
        mp_event_handle_nowait();
        // TODO check return value
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    mp_uint_t atomic_state = begin_critical_flash_section();
    flash_range_program(self->flash_base + offset, bufinfo.buf, bufinfo.len);
    end_critical_flash_section(atomic_state);
    mp_event_handle_nowait();
    // TODO check return value
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_flash_writeblocks_obj, 3, 4, rp2_flash_writeblocks);

static mp_obj_t rp2_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * BLOCK_SIZE_BYTES;
            mp_uint_t atomic_state = begin_critical_flash_section();
            flash_range_erase(self->flash_base + offset, BLOCK_SIZE_BYTES);
            end_critical_flash_section(atomic_state);
            // TODO check return value
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(rp2_flash_ioctl_obj, rp2_flash_ioctl);

static const mp_rom_map_elem_t rp2_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&rp2_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&rp2_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&rp2_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(rp2_flash_locals_dict, rp2_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, rp2_flash_make_new,
    locals_dict, &rp2_flash_locals_dict
    );
