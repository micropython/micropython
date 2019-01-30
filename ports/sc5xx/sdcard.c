/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

#include "sdcard.h"
#include "pin.h"
#include "rsi.h"

#if MICROPY_HW_HAS_SDCARD

// If no custom SDIO pins defined, use the default ones
#ifndef MICROPY_HW_MSI0_CLK

#define MICROPY_HW_MSI0_D0 (pin_F2)
#define MICROPY_HW_MSI0_D1 (pin_F3)
#define MICROPY_HW_MSI0_D2 (pin_F4)
#define MICROPY_HW_MSI0_D3 (pin_F5)
#define MICROPY_HW_MSI0_CLK (pin_F11)
#define MICROPY_HW_MSI0_CMD (pin_F10)
#define MICROPY_HW_MSI0_CD (pin_F12)

#define AF0_MSI0 0

#endif

SD_MMC_CARD_TYPE type;

void sdcard_init(void) {
    // invalidate the sd_handle
    type = UNUSABLE_CARD;

    // configure SD GPIO
    // Default SDIO/SDMMC1 config
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_D0, GPIO_MODE_AF_PP, 0, AF0_MSI0);
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_D1, GPIO_MODE_AF_PP, 0, AF0_MSI0);
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_D2, GPIO_MODE_AF_PP, 0, AF0_MSI0);
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_D3, GPIO_MODE_AF_PP, 0, AF0_MSI0);
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_CLK, GPIO_MODE_AF_PP, 0, AF0_MSI0);
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_CMD, GPIO_MODE_AF_PP, 0, AF0_MSI0);

    // configure the SD card detect pin
    mp_hal_pin_config((pin_obj_t *)MICROPY_HW_MSI0_CD, GPIO_MODE_AF_PP, 0, AF0_MSI0);

    // open the device so it is possible to query card presence.
    adi_rsi_Open(0, &hDevice);
}

bool sdcard_is_present(void) {
    return adi_rsi_IsCardPresent(hDevice);
}

bool sdcard_power_on(void) {
    if (type != UNUSABLE_CARD) {
        return true;
    }

    if(sdcard_is_present())
    {
        RSI_GetClocks();

        /* wait for a card to be detected and configure the RSI for card identification */
        RSI_WaitForCard();

        /* Set 1 bit bus width transfer for identification */
        adi_rsi_SetBusWidth(hDevice, 1);

        /* identify the card type and initialize the card if it's compatible         */
        type = sd_mmc_identification();

        if (type != UNUSABLE_CARD)
            return true;
    }

    return false;
}

void sdcard_power_off(void) {
    if (type == UNUSABLE_CARD) {
        return;
    }
    adi_rsi_Close(hDevice);
    type = UNUSABLE_CARD;
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    if (type == UNUSABLE_CARD) {
        return 0;
    }
    return RSI_GetSDCapacity();
}

mp_uint_t sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    // check that SD card is initialised
    if (type == UNUSABLE_CARD) {
        return 1;
    }

    // check that dest pointer is aligned on a 4-byte boundary
    uint8_t *orig_dest = NULL;
    uint32_t saved_word;
    if (((uint32_t)dest & 3) != 0) {
        // Pointer is not aligned so it needs fixing.
        // We could allocate a temporary block of RAM (as sdcard_write_blocks
        // does) but instead we are going to use the dest buffer inplace.  We
        // are going to align the pointer, save the initial word at the aligned
        // location, read into the aligned memory, move the memory back to the
        // unaligned location, then restore the initial bytes at the aligned
        // location.  We should have no trouble doing this as those initial
        // bytes at the aligned location should be able to be changed for the
        // duration of this function call.
        orig_dest = dest;
        dest = (uint8_t*)((uint32_t)dest & ~3);
        saved_word = *(uint32_t*)dest;
    }

    uint32_t result = RSI_ReadMultipleBlocks(block_num, dest, num_blocks);

    if (orig_dest != NULL) {
        // move the read data to the non-aligned position, and restore the initial bytes
        memmove(orig_dest, dest, num_blocks * SDCARD_BLOCK_SIZE);
        memcpy(dest, &saved_word, orig_dest - dest);
    }

    return result;
}

mp_uint_t sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    // check that SD card is initialised
    if (type == UNUSABLE_CARD) {
        return 1;
    }
    uint32_t result;

    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        // pointer is not aligned, so allocate a temporary block to do the write
        uint8_t *src_aligned = m_new_maybe(uint8_t, SDCARD_BLOCK_SIZE);
        if (src_aligned == NULL) {
            return 1;
        }
        for (size_t i = 0; i < num_blocks; ++i) {
            memcpy(src_aligned, src + i * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
            result = sdcard_write_blocks(src_aligned, block_num + i, 1);
            if (result != 0) {
                break;
            }
        }
        m_del(uint8_t, src_aligned, SDCARD_BLOCK_SIZE);
        return result;
    }

    result = RSI_WriteMultipleBlocks(block_num, (void *)src, num_blocks);

    return result;
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the SD card as an object with the block protocol.

// there is a singleton SDCard object
const mp_obj_base_t sdcard_obj = {&sdcard_type};

STATIC mp_obj_t sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return MP_OBJ_FROM_PTR(&sdcard_obj);
}

STATIC mp_obj_t sd_present(mp_obj_t self) {
    return mp_obj_new_bool(sdcard_is_present());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_present_obj, sd_present);

STATIC mp_obj_t sd_power(mp_obj_t self, mp_obj_t state) {
    bool result;
    if (mp_obj_is_true(state)) {
        result = sdcard_power_on();
    } else {
        sdcard_power_off();
        result = true;
    }
    return mp_obj_new_bool(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_power_obj, sd_power);

STATIC mp_obj_t sd_info(mp_obj_t self) {
    if (type == UNUSABLE_CARD) {
        return mp_const_none;
    }

    // cardinfo.SD_csd and cardinfo.SD_cid have lots of info but we don't use them
    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull(sdcard_get_capacity_in_bytes()),
        mp_obj_new_int_from_uint(512),
        mp_obj_new_int(type),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

// now obsolete, kept for backwards compatibility
STATIC mp_obj_t sd_read(mp_obj_t self, mp_obj_t block_num) {
    uint8_t *dest = m_new(uint8_t, SDCARD_BLOCK_SIZE);
    mp_uint_t ret = sdcard_read_blocks(dest, mp_obj_get_int(block_num), 1);

    if (ret != 0) {
        m_del(uint8_t, dest, SDCARD_BLOCK_SIZE);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_read_blocks failed [%u]", ret));
    }

    return mp_obj_new_bytearray_by_ref(SDCARD_BLOCK_SIZE, dest);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_read_obj, sd_read);

// now obsolete, kept for backwards compatibility
STATIC mp_obj_t sd_write(mp_obj_t self, mp_obj_t block_num, mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len % SDCARD_BLOCK_SIZE != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "writes must be a multiple of %d bytes", SDCARD_BLOCK_SIZE));
    }

    mp_uint_t ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);

    if (ret != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_write_blocks failed [%u]", ret));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sd_write_obj, sd_write);

STATIC mp_obj_t sdcard_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = sdcard_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sdcard_readblocks_obj, sdcard_readblocks);

STATIC mp_obj_t sdcard_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sdcard_writeblocks_obj, sdcard_writeblocks);

STATIC mp_obj_t sdcard_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:
            if (!sdcard_power_on()) {
                return MP_OBJ_NEW_SMALL_INT(-1); // error
            }
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_DEINIT:
            sdcard_power_off();
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(sdcard_get_capacity_in_bytes() / SDCARD_BLOCK_SIZE);

        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SDCARD_BLOCK_SIZE);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sdcard_ioctl_obj, sdcard_ioctl);

STATIC const mp_rom_map_elem_t sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_present), MP_ROM_PTR(&sd_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_power), MP_ROM_PTR(&sd_power_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&sd_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&sd_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&sd_write_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&sdcard_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

const mp_obj_type_t sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = sdcard_make_new,
    .locals_dict = (mp_obj_dict_t*)&sdcard_locals_dict,
};

void sdcard_init_vfs(fs_user_mount_t *vfs, int part) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = part;
    vfs->readblocks[0] = MP_OBJ_FROM_PTR(&sdcard_readblocks_obj);
    vfs->readblocks[1] = MP_OBJ_FROM_PTR(&sdcard_obj);
    vfs->readblocks[2] = MP_OBJ_FROM_PTR(sdcard_read_blocks); // native version
    vfs->writeblocks[0] = MP_OBJ_FROM_PTR(&sdcard_writeblocks_obj);
    vfs->writeblocks[1] = MP_OBJ_FROM_PTR(&sdcard_obj);
    vfs->writeblocks[2] = MP_OBJ_FROM_PTR(sdcard_write_blocks); // native version
    vfs->u.ioctl[0] = MP_OBJ_FROM_PTR(&sdcard_ioctl_obj);
    vfs->u.ioctl[1] = MP_OBJ_FROM_PTR(&sdcard_obj);
}

#endif // MICROPY_HW_HAS_SDCARD
