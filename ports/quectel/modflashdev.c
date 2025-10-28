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

#include "py/runtime.h"

#if MICROPY_VFS
#include "extmod/vfs.h"
#endif

#include "helios_flash.h"
#include "helios_debug.h"

#define MOD_FLASHDEV_LOG(msg, ...)      custom_log('flashdev', msg, ##__VA_ARGS__)

const mp_obj_type_t helios_flash_device_type;

struct lfs_flash_info
{
    uint32_t FlashType;
    uint32_t LfsStartAddress;
    uint32_t LfsEndAddress;
};

typedef struct _helios_flash_device_obj_t {
    mp_obj_base_t base;
    int block_size;
    int block_count;
    struct lfs_flash_info info;
    char partition_name[32];
} helios_flash_device_obj_t;

static mp_obj_t helios_flash_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    helios_flash_device_obj_t *self = mp_obj_malloc(helios_flash_device_obj_t, type);

    mp_buffer_info_t bufinfo;
    memset(self->partition_name, 0x0, sizeof(self->partition_name));
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
    if( bufinfo.len >= sizeof(self->partition_name)) {
        mp_raise_ValueError(MP_ERROR_TEXT("partition name is invalid"));
    }
    memcpy(self->partition_name, bufinfo.buf, bufinfo.len);
    
    self->block_size = mp_obj_get_int(args[1]);

    if (self->block_size <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid block size"));
    }

    HeliosFlashPartiCtx *FlashPartiCtx = Helios_Flash_GetPartiCtx((const char*)self->partition_name);
    if(!FlashPartiCtx) mp_raise_OSError(19);

    uint32_t FlashPartiAddr = FlashPartiCtx->addr;
    size_t FlashPartiSize = FlashPartiCtx->size;
    MOD_FLASHDEV_LOG("flash addr:%x", FlashPartiAddr);
    MOD_FLASHDEV_LOG("flash size:%x", FlashPartiSize);
	self->info.LfsStartAddress = (FlashPartiAddr) / self->block_size * self->block_size;
#if MICROPY_VFS_LFS2
    self->info.LfsEndAddress = ((FlashPartiAddr + FlashPartiSize) / self->block_size) * self->block_size;
#else
	self->info.LfsEndAddress = ((FlashPartiAddr + FlashPartiSize - 1) / self->block_size) * self->block_size;
#endif

    self->block_count = FlashPartiSize / self->block_size;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t helios_flash_device_readblocks(size_t n_args, const mp_obj_t *args)
{
    helios_flash_device_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t FlashAddrss = 0;
    uint32_t block_num = mp_obj_get_int(args[1]);
    uint32_t offset = block_num * self->block_size;
    mp_buffer_info_t bufinfo;
    int ret;

    if (n_args >= 4) {
        offset += mp_obj_get_int(args[3]);
    }

    FlashAddrss = self->info.LfsStartAddress + offset;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    ret = Helios_Flash_Read((uint32_t)FlashAddrss, bufinfo.buf, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(helios_flash_device_readblocks_obj, 3, 4, helios_flash_device_readblocks);

static mp_obj_t helios_flash_device_writeblocks(size_t n_args, const mp_obj_t *args)
{
    helios_flash_device_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    uint32_t offset = block_num * self->block_size;
    mp_buffer_info_t bufinfo;
    int ret;
    uint32_t FlashAddrss = self->info.LfsStartAddress + offset;

    if (n_args == 3) {
        ret = Helios_Flash_Erase(FlashAddrss, self->block_size);
        if (ret) {
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    FlashAddrss = self->info.LfsStartAddress + offset;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    ret = Helios_Flash_Write(FlashAddrss, bufinfo.buf, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(helios_flash_device_writeblocks_obj, 3, 4, helios_flash_device_writeblocks);

static mp_obj_t helios_flash_device_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in)
{
    helios_flash_device_obj_t *self = self_in;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    mp_int_t block_num = mp_obj_get_int(arg_in);
    int ret;
    uint32_t FlashAddrss;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->block_count);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->block_size);

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE:
            FlashAddrss = self->info.LfsStartAddress + (block_num * self->block_size);
            ret = Helios_Flash_Erase(FlashAddrss, self->block_size);
            return MP_OBJ_NEW_SMALL_INT(ret);

        default:
            return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(helios_flash_device_ioctl_obj, helios_flash_device_ioctl);

static const mp_rom_map_elem_t helios_flash_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&helios_flash_device_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&helios_flash_device_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&helios_flash_device_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(helios_flash_device_locals_dict, helios_flash_device_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    helios_flash_device_type,
    MP_QSTR_FlashDevice,
    MP_TYPE_FLAG_NONE,
    make_new, helios_flash_device_make_new,
    locals_dict, &helios_flash_device_locals_dict
    );
