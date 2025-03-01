/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

#if !BUILDING_MBOOT

#include "py/runtime.h"
#include "extmod/vfs.h"

#include "storage.h"

// Expose the entire external SPI flash as an object with the block protocol.

#define FLASH_SIZE (MICROPY_HW_SPIFLASH_OFFSET_BYTES + MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define BLOCK_SIZE (4096)

typedef struct _spi_flash_obj_t {
    mp_obj_base_t base;
    uint32_t start; // in bytes
    uint32_t len; // in bytes
} spi_flash_obj_t;

static const mp_obj_type_t spi_flash_type;

static void spi_flash_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    spi_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPIFlash(start=%u, len=%u)", self->start, self->len);
}

static mp_obj_t spi_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    spi_flash_obj_t *self = mp_obj_malloc(spi_flash_obj_t, &spi_flash_type);

    mp_int_t start = args[ARG_start].u_int;
    if (!(0 <= start && start < FLASH_SIZE && start % BLOCK_SIZE == 0)) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = FLASH_SIZE - start;
    } else if (!(0 < len && start + len <= FLASH_SIZE && len % BLOCK_SIZE == 0)) {
        mp_raise_ValueError(NULL);
    }

    self->start = start;
    self->len = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t spi_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    spi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = self->start / BLOCK_SIZE + mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    int ret = spi_bdev_readblocks_raw(&spi_bdev, bufinfo.buf, block_num, 0, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(spi_flash_readblocks_obj, 3, 3, spi_flash_readblocks);

static mp_obj_t spi_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    spi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = self->start / BLOCK_SIZE + mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    int ret = spi_bdev_eraseblocks_raw(&spi_bdev, block_num, bufinfo.len);
    if (ret == 0) {
        ret = spi_bdev_writeblocks_raw(&spi_bdev, bufinfo.buf, block_num, 0, bufinfo.len);
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(spi_flash_writeblocks_obj, 3, 3, spi_flash_writeblocks);

static mp_obj_t spi_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    spi_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            storage_init();
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: {
            mp_int_t n = self->len / BLOCK_SIZE;
            return MP_OBJ_NEW_SMALL_INT(n);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(BLOCK_SIZE);

        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_ioctl_obj, spi_flash_ioctl);

static const mp_rom_map_elem_t spi_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&spi_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&spi_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&spi_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(spi_flash_locals_dict, spi_flash_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    spi_flash_type,
    MP_QSTR_SPIFlash,
    MP_TYPE_FLAG_NONE,
    make_new, spi_flash_make_new,
    print, spi_flash_print,
    locals_dict, &spi_flash_locals_dict
    );

/******************************************************************************/
// The `spiflash` module.

static const mp_rom_map_elem_t spiflash_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_spiflash) },

    { MP_ROM_QSTR(MP_QSTR_SPIFlash), MP_ROM_PTR(&spi_flash_type) },
};
static MP_DEFINE_CONST_DICT(spiflash_module_globals, spiflash_module_globals_table);

const mp_obj_module_t spiflash_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&spiflash_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_spiflash, spiflash_module);

#endif
