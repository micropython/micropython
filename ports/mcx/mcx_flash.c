/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "py/runtime.h"
#include "extmod/vfs.h"

#include "drv_iflash.h"

#include "modmcx.h"

typedef enum {
    MCX_FLASH_TYPE_INTERNAL,
    #if MICROPY_HW_ENABLE_EFS
    MCX_FLASH_TYPE_EXTERNAL,
    #endif
    MCX_FLASH_TYPE_END,
} mcx_flash_type_t;

typedef struct mcx_flash_obj_type {
    mp_obj_base_t base;
    mcx_flash_type_t type;
    drv_iflash_t iflash;
} mcx_flash_obj_t;

static mp_obj_t mcx_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, true);

    mcx_flash_obj_t *self = mp_obj_malloc(mcx_flash_obj_t, &mcx_flash_type);

    if (n_args > 1) {
        self->type = mp_obj_get_int(args[1]);
    }

    if (self->type >= MCX_FLASH_TYPE_END) {
        self->type = MCX_FLASH_TYPE_INTERNAL;
    }

    if (self->type == MCX_FLASH_TYPE_INTERNAL) {
        if (drv_iflash_init(&self->iflash) < 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize Flash API"));
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

static void mcx_flash_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mcx_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    qstr type_qst;

    switch (self->type) {
        #if MICROPY_HW_ENABLE_EFS
        case MCX_FLASH_TYPE_EXTERNAL:
            type_qst = MP_QSTR_EXTERNAL;
            break;
        #endif

        default:
        case MCX_FLASH_TYPE_INTERNAL:
            type_qst = MP_QSTR_INTERNAL;
            break;
    }

    mp_printf(print, "Flash(Flash.%s)", qstr_str(type_qst));
}

static mp_obj_t mcx_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mcx_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    uint32_t block_id = mp_obj_get_int(args[1]);
    uint32_t block_off = 0;

    if (n_args == 4) {
        block_off = mp_obj_get_int(args[3]);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    if (self->type == MCX_FLASH_TYPE_INTERNAL) {
        drv_iflash_attr_t attr;
        drv_iflash_attr_get(&self->iflash, &attr);

        uint32_t flash_addr = MICROPY_HW_IFS_START + attr.sector_size * block_id + block_off;

        drv_iflash_read(&self->iflash, flash_addr, bufinfo.buf, bufinfo.len);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcx_flash_readblocks_obj, 3, 4, mcx_flash_readblocks);


static mp_obj_t mcx_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    mcx_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    uint32_t block_id = mp_obj_get_int(args[1]);
    uint32_t block_off = 0;

    if (n_args == 4) {
        block_off = mp_obj_get_int(args[3]);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    if (self->type == MCX_FLASH_TYPE_INTERNAL) {
        drv_iflash_attr_t attr;
        drv_iflash_attr_get(&self->iflash, &attr);

        uint32_t page_addr = MICROPY_HW_IFS_START + block_id * attr.sector_size + block_off;
        uint32_t page_count = bufinfo.len / attr.page_size;

        for (uint32_t i = 0; i < page_count; i++) {
            uint8_t *buf_ptr = &((uint8_t *)bufinfo.buf)[i * attr.page_size];
            int ret = drv_iflash_program_page(&self->iflash, page_addr + i * attr.page_size, buf_ptr);
            if (ret < 0) {
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("program phrase failed: %d"), ret);
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcx_flash_writeblocks_obj, 3, 4, mcx_flash_writeblocks);

static mp_obj_t mcx_flash_progsize(mp_obj_t self_in) {
    mcx_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->type == MCX_FLASH_TYPE_INTERNAL) {
        drv_iflash_attr_t attr;
        drv_iflash_attr_get(&self->iflash, &attr);

        return MP_OBJ_NEW_SMALL_INT(attr.page_size);
    }

    return MP_OBJ_NEW_SMALL_INT(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcx_flash_progsize_obj, mcx_flash_progsize);

static mp_obj_t mcx_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mcx_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint32_t erase_size = 0U;
    uint32_t block_count = 0U;
    uint32_t block_id = mp_obj_get_int(arg_in);

    if (self->type == MCX_FLASH_TYPE_INTERNAL) {
        drv_iflash_attr_t attr;
        drv_iflash_attr_get(&self->iflash, &attr);

        erase_size = attr.sector_size;
        block_count = MICROPY_HW_IFS_LENGTH / erase_size;
    }

    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(block_count);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(erase_size);

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            if (self->type == MCX_FLASH_TYPE_INTERNAL) {
                if (drv_iflash_erase_sector(&self->iflash, MICROPY_HW_IFS_START + block_id * erase_size) < 0) {
                    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Erase sector failed."));
                }
            }

            return MP_OBJ_NEW_SMALL_INT(0);
        }

        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(mcx_flash_ioctl_obj, mcx_flash_ioctl);

static const mp_rom_map_elem_t mcx_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&mcx_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&mcx_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_progsize), MP_ROM_PTR(&mcx_flash_progsize_obj)},
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mcx_flash_ioctl_obj) },

    /* Constants */
    { MP_ROM_QSTR(MP_QSTR_INTERNAL), MP_ROM_INT(MCX_FLASH_TYPE_INTERNAL) },
    #if MICROPY_HW_ENABLE_EFS
    { MP_ROM_QSTR(MP_QSTR_EXTERNAL), MP_ROM_INT(MCX_FLASH_TYPE_EXTERNAL) },
    #endif
};
static MP_DEFINE_CONST_DICT(mcx_flash_locals_dict, mcx_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mcx_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, mcx_flash_make_new,
    print, mcx_flash_print,
    locals_dict, &mcx_flash_locals_dict
    );
