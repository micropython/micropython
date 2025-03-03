/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ayke van Laethem
 * Copyright (c) 2019 Glenn Ruben Bakke
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
#include <string.h>
#include "py/runtime.h"
#include "py/mperrno.h"

#if MICROPY_PY_NRF && MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

#include "flashbdev.h"
#include "flash.h"
#include "extmod/vfs.h"

extern byte _fs_start[];
extern byte _fs_end[];

#define FLASH_BLOCK_START (((uint32_t)_fs_start + (FLASH_PAGESIZE - 1)) / FLASH_PAGESIZE)

typedef struct _nrf_flash_obj_t {
    mp_obj_base_t base;
    uint32_t start; // in bytes
    uint32_t len; // in bytes
} nrf_flash_obj_t;

// This flash object represents the flash region defined by _fs_start and _fs_end
// in memory.ld.  The start/len entries will be filled in by flashbdev_init().
nrf_flash_obj_t nrf_flash_obj = {
    { &nrf_flashbdev_type },
    0,
    0,
};

mp_obj_t nrf_flashbdev_readblocks(size_t n_args, const mp_obj_t *args) {
    nrf_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    mp_int_t address = self->start + (block_num * FLASH_PAGESIZE);

    if (n_args == 4) {
        uint32_t offset = mp_obj_get_int(args[3]);
        address += offset;
    }

    byte *buf = bufinfo.buf;
    byte *p = (byte *)address;
    memcpy(buf, p, bufinfo.len);

    return MP_OBJ_NEW_SMALL_INT(0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(nrf_flashbdev_readblocks_obj, 3, 4, nrf_flashbdev_readblocks);

mp_obj_t nrf_flashbdev_writeblocks(size_t n_args, const mp_obj_t *args) {
    nrf_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    mp_int_t address = self->start + (block_num * FLASH_PAGESIZE);

    if (n_args == 4) {
        uint32_t offset = mp_obj_get_int(args[3]);
        address += offset;
    } else {
        flash_page_erase(address);
    }

    flash_write_bytes(address, bufinfo.buf, bufinfo.len);

    return MP_OBJ_NEW_SMALL_INT(0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(nrf_flashbdev_writeblocks_obj, 3, 4, nrf_flashbdev_writeblocks);

mp_obj_t nrf_flashbdev_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    nrf_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t op = mp_obj_get_int(op_in);
    switch (op) {
        case MP_BLOCKDEV_IOCTL_INIT: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }

        case MP_BLOCKDEV_IOCTL_DEINIT: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }

        case MP_BLOCKDEV_IOCTL_SYNC: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: {
            return MP_OBJ_NEW_SMALL_INT(self->len / FLASH_PAGESIZE);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE: {
            return MP_OBJ_NEW_SMALL_INT(FLASH_PAGESIZE);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            mp_int_t block_num = mp_obj_get_int(arg_in);
            mp_int_t address = self->start + (block_num * FLASH_PAGESIZE);

            if ((address & 0x3) || (address % FLASH_PAGESIZE != 0)) {
                return MP_OBJ_NEW_SMALL_INT(-MP_EIO);
            }

            flash_page_erase(address);
            return MP_OBJ_NEW_SMALL_INT(0);
        }

        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(nrf_flashbdev_ioctl_obj, nrf_flashbdev_ioctl);

static const mp_rom_map_elem_t nrf_flashbdev_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&nrf_flashbdev_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&nrf_flashbdev_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&nrf_flashbdev_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(nrf_flashbdev_locals_dict, nrf_flashbdev_locals_dict_table);

static void nrf_flashbdev_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    nrf_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Flash(start=0x%08x, len=%u)", self->start, self->len);
}

static mp_obj_t nrf_flashbdev_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        return MP_OBJ_FROM_PTR(&nrf_flash_obj);
    }

    nrf_flash_obj_t *self = mp_obj_malloc(nrf_flash_obj_t, &nrf_flashbdev_type);

    mp_int_t start = args[ARG_start].u_int;
    mp_int_t len = args[ARG_len].u_int;

    if ((start == -1) || (start % FLASH_PAGESIZE != 0)) {
        mp_raise_ValueError(NULL);
    }
    if ((len == -1) || (len % FLASH_PAGESIZE != 0)) {
        mp_raise_ValueError(NULL);
    }

    self->start = start;
    self->len = len;

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    nrf_flashbdev_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, nrf_flashbdev_make_new,
    print, nrf_flashbdev_print,
    locals_dict, &nrf_flashbdev_locals_dict
    );

void flashbdev_init(void) {
    // Set start to first aligned page from _fs_start.
    mp_int_t page_start = FLASH_BLOCK_START;
    nrf_flash_obj.start = page_start * FLASH_PAGESIZE;
    // Trim len to only count whole pages.
    mp_int_t page_end = (uint32_t)_fs_end / FLASH_PAGESIZE;
    mp_int_t num_pages = page_end - page_start;
    nrf_flash_obj.len = num_pages * FLASH_PAGESIZE;
}

#endif // MICROPY_PY_NRF && MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
