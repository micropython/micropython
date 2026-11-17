/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modalif.h"
#include "ospi_flash.h"

#if MICROPY_HW_ENABLE_OSPI
typedef struct _alif_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base_addr;
    uint32_t flash_size;
} alif_flash_obj_t;

static const alif_flash_obj_t alif_flash_fs_obj = {
    .base = { &alif_flash_type },
    .flash_base_addr = MICROPY_HW_FLASH_STORAGE_BASE_ADDR,
    .flash_size = MICROPY_HW_FLASH_STORAGE_FS_BYTES,
};

static mp_obj_t alif_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        // Default singleton object that accesses writable-filesystem flash
        return MP_OBJ_FROM_PTR(&alif_flash_fs_obj);
    }

    alif_flash_obj_t *self = mp_obj_malloc(alif_flash_obj_t, &alif_flash_type);

    mp_int_t start = args[ARG_start].u_int;
    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % MICROPY_HW_FLASH_BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % MICROPY_HW_FLASH_BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    self->flash_base_addr = MICROPY_HW_FLASH_STORAGE_BASE_ADDR + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_int_t alif_flash_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    alif_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (flags == MP_BUFFER_READ) {
        bufinfo->buf = (void *)(ospi_flash_get_xip_base() + self->flash_base_addr);
        bufinfo->len = self->flash_size;
        bufinfo->typecode = 'B';
        return 0;
    } else {
        // Can't return a writable buffer.
        return 1;
    }
}

static mp_obj_t alif_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    alif_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    int ret = ospi_flash_read(self->flash_base_addr + offset, bufinfo.len, bufinfo.buf);
    mp_event_handle_nowait();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alif_flash_readblocks_obj, 3, 4, alif_flash_readblocks);

static mp_obj_t alif_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    alif_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        uint32_t addr = self->flash_base_addr + offset;
        int32_t len = bufinfo.len;
        while (len > 0) {
            int ret = ospi_flash_erase_sector(addr);
            mp_event_handle_nowait();
            if (ret < 0) {
                return MP_OBJ_NEW_SMALL_INT(ret);
            }
            addr += MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
            len -= MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
        }
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    int ret = ospi_flash_write(self->flash_base_addr + offset, bufinfo.len, bufinfo.buf);
    mp_event_handle_nowait();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alif_flash_writeblocks_obj, 3, 4, alif_flash_writeblocks);

static mp_obj_t alif_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    alif_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / MICROPY_HW_FLASH_BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(MICROPY_HW_FLASH_BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
            int ret = ospi_flash_erase_sector(self->flash_base_addr + offset);
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(alif_flash_ioctl_obj, alif_flash_ioctl);

static const mp_rom_map_elem_t alif_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&alif_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&alif_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&alif_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(alif_flash_locals_dict, alif_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    alif_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, alif_flash_make_new,
    buffer, alif_flash_get_buffer,
    locals_dict, &alif_flash_locals_dict
    );
#endif // MICROPY_HW_ENABLE_OSPI
