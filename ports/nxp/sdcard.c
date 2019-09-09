/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#include <stdio.h>
#include <string.h>

#include "sdcard.h"

sd_card_t g_sd;

status_t ReadBlocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks)
{
    return SD_ReadBlocks(&g_sd, dest, block_num, num_blocks);
}

status_t WriteBlocks(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    return SD_WriteBlocks(&g_sd, buffer, startBlock, blockCount);
}

bool disk_initialize(void)
{
    if(g_sd.isHostReady)
    {
        SD_HostReset(&(g_sd.host));
    }
    else 
    {
        return 0;
    }
    if(kStatus_Success != SD_CardInit(&g_sd))
    {
        SD_CardDeinit(&g_sd);
        memset(&g_sd, 0U, sizeof(g_sd));
        return 0;
    }
    return 1;
}

/* Micropython bindings */
const mp_obj_base_t pyb_sdcard_obj = {&pyb_sdcard_type};

STATIC mp_obj_t pyb_sd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}

STATIC mp_obj_t sd_read(mp_obj_t self, mp_obj_t block_num)
{
    uint8_t *buffer = m_new(uint8_t, 512);
    status_t ret = SD_ReadBlocks(&g_sd, buffer, mp_obj_get_int(block_num), 1);
    if (ret != kStatus_Success )
    {
        m_del(uint8_t,buffer, 512);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_read_blocks failed [%u]", ret));
    }
    return mp_obj_new_bytearray_by_ref(512, buffer);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_read_obj, sd_read);

STATIC mp_obj_t pyb_sd_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf)
{
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    status_t ret = SD_ReadBlocks(&g_sd, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len/512);
    return mp_obj_new_bool(ret == kStatus_Success);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_readblocks_obj, pyb_sd_readblocks);

STATIC mp_obj_t sd_write(mp_obj_t self, mp_obj_t block_num, mp_obj_t data)
{
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    if(bufinfo.len % 512 != 0)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "writes must be a multiple of %d bytes", 512));
    }
    status_t ret = SD_WriteBlocks(&g_sd, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len/512);

    if(ret != kStatus_Success)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_write_blocks failed [%u]", ret));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sd_write_obj, sd_write);

STATIC mp_obj_t pyb_sd_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf)
{
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    status_t ret = SD_WriteBlocks(&g_sd, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len/512);
    return mp_obj_new_bool(ret == kStatus_Success);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_writeblocks_obj, pyb_sd_writeblocks);

STATIC mp_obj_t pyb_sd_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in)
{
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:
            if (disk_initialize() != 1) {
                return MP_OBJ_NEW_SMALL_INT(-1); // error
            }
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_DEINIT:
            SD_CardDeinit(&g_sd);
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(0);

        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(512);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_ioctl_obj, pyb_sd_ioctl);


STATIC const mp_rom_map_elem_t pyb_sdcard_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&sd_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&sd_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_sd_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_sd_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_sd_ioctl_obj) },
};

MP_DEFINE_CONST_DICT(pyb_sdcard_locals_dict, pyb_sdcard_locals_table);

const mp_obj_type_t pyb_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = pyb_sd_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_sdcard_locals_dict,
};

void sdcard_init_vfs(fs_user_mount_t *vfs, int part)
{
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = part;
    vfs->readblocks[0] = MP_OBJ_FROM_PTR(&pyb_sd_readblocks_obj);
    vfs->readblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->readblocks[2] = MP_OBJ_FROM_PTR(ReadBlocks);   /* native version */
    vfs->writeblocks[0] = MP_OBJ_FROM_PTR(&pyb_sd_writeblocks_obj);
    vfs->writeblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->writeblocks[2] = MP_OBJ_FROM_PTR(WriteBlocks); /* native version */
    vfs->u.ioctl[0] = MP_OBJ_FROM_PTR(&pyb_sd_ioctl_obj);
    vfs->u.ioctl[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}