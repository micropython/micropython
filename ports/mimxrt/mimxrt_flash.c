/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021-2023 Philipp Ebensberger
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

#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modmimxrt.h"
#include "flash.h"
#include BOARD_FLASH_OPS_HEADER_H


extern flexspi_nor_config_t qspiflash_config;

typedef struct _mimxrt_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} mimxrt_flash_obj_t;

static mimxrt_flash_obj_t mimxrt_flash_obj = {
    .base = { &mimxrt_flash_type }
};

static mp_obj_t mimxrt_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check args.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    flash_init();

    // Update information based on linker symbols.
    mimxrt_flash_obj.flash_base = MICROPY_HW_FLASH_STORAGE_BASE;
    mimxrt_flash_obj.flash_size = MICROPY_HW_FLASH_STORAGE_BYTES;

    // Return singleton object.
    return MP_OBJ_FROM_PTR(&mimxrt_flash_obj);
}

// readblocks(block_num, buf, [offset])
// read size of buffer number of bytes from block (with offset) into buffer
static mp_obj_t mimxrt_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    // Calculate read offset from block number.
    uint32_t offset = mp_obj_get_int(args[1]) * SECTOR_SIZE_BYTES;
    // Add optional offset
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }

    flash_read_block((self->flash_base + offset), (uint8_t *)bufinfo.buf, (uint32_t)bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mimxrt_flash_readblocks_obj, 3, 4, mimxrt_flash_readblocks);

// writeblocks(block_num, buf, [offset])
// Erase block based on block_num and write buffer size number of bytes from buffer into block. If additional offset
// parameter is provided only write operation at block start + offset will be performed.
// This requires a prior erase operation of the block!
static mp_obj_t mimxrt_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    status_t status;
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    // Calculate read offset from block number.
    uint32_t offset = mp_obj_get_int(args[1]) * SECTOR_SIZE_BYTES;

    if (n_args == 3) {
        status = flash_erase_sector(self->flash_base + offset);

        if (status != kStatus_Success) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("flash erase command failed with %d"), status);
        }
    } else {
        // Add optional offset
        offset += mp_obj_get_int(args[3]);
    }

    status = flash_write_block(self->flash_base + offset, bufinfo.buf, bufinfo.len);

    if (status != kStatus_Success) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("flash block write command failed with %d"), status);
    }

    return MP_OBJ_NEW_SMALL_INT(status != kStatus_Success);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mimxrt_flash_writeblocks_obj, 3, 4, mimxrt_flash_writeblocks);

// ioctl(op, arg)
static mp_obj_t mimxrt_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    status_t status;
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / SECTOR_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SECTOR_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * SECTOR_SIZE_BYTES;
            status = flash_erase_sector(self->flash_base + offset);
            return MP_OBJ_NEW_SMALL_INT(status != kStatus_Success);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(mimxrt_flash_ioctl_obj, mimxrt_flash_ioctl);

static const mp_rom_map_elem_t mimxrt_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&mimxrt_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&mimxrt_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mimxrt_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(mimxrt_flash_locals_dict, mimxrt_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mimxrt_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, mimxrt_flash_make_new,
    locals_dict, &mimxrt_flash_locals_dict
    );
