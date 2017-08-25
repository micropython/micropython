/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "internal_flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include "asf/sam0/drivers/nvm/nvm.h"
#include "asf/sam0/drivers/port/port.h"

#include "rgb_led_status.h"

void internal_flash_init(void) {
    // Activity LED for flash writes.
    #ifdef MICROPY_HW_LED_MSC
        struct port_config pin_conf;
        port_get_config_defaults(&pin_conf);

        pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
        port_pin_set_config(MICROPY_HW_LED_MSC, &pin_conf);
        port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
    #endif
}

uint32_t internal_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t internal_flash_get_block_count(void) {
    return INTERNAL_FLASH_PART1_START_BLOCK + INTERNAL_FLASH_PART1_NUM_BLOCKS;
}

void internal_flash_flush(void) {
}

void flash_flush(void) {
    internal_flash_flush();
}

static void build_partition(uint8_t *buf, int boot, int type, uint32_t start_block, uint32_t num_blocks) {
    buf[0] = boot;

    if (num_blocks == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_blocks == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_block;
    buf[9] = start_block >> 8;
    buf[10] = start_block >> 16;
    buf[11] = start_block >> 24;

    buf[12] = num_blocks;
    buf[13] = num_blocks >> 8;
    buf[14] = num_blocks >> 16;
    buf[15] = num_blocks >> 24;
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (INTERNAL_FLASH_PART1_START_BLOCK <= block && block < INTERNAL_FLASH_PART1_START_BLOCK + INTERNAL_FLASH_PART1_NUM_BLOCKS) {
        // a block in partition 1
        block -= INTERNAL_FLASH_PART1_START_BLOCK;
        return INTERNAL_FLASH_MEM_SEG1_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool internal_flash_read_block(uint8_t *dest, uint32_t block) {
    if (block == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, INTERNAL_FLASH_PART1_START_BLOCK, INTERNAL_FLASH_PART1_NUM_BLOCKS);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;

    } else {
        // non-MBR block, get data from flash memory
        int32_t src = convert_block_to_flash_addr(block);
        if (src == -1) {
            // bad block number
            return false;
        }
        enum status_code error_code;
        // A block is made up of multiple pages. Read each page
        // sequentially.
        for (int i = 0; i < FILESYSTEM_BLOCK_SIZE / NVMCTRL_PAGE_SIZE; i++) {
          do
          {
              error_code = nvm_read_buffer(src + i * NVMCTRL_PAGE_SIZE,
                                   dest + i * NVMCTRL_PAGE_SIZE,
                                   NVMCTRL_PAGE_SIZE);
          } while (error_code == STATUS_BUSY);
        }
        return true;
    }
}

bool internal_flash_write_block(const uint8_t *src, uint32_t block) {
    if (block == 0) {
        // can't write MBR, but pretend we did
        return true;

    } else {
        #ifdef MICROPY_HW_LED_MSC
            port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
        #endif
        temp_status_color(ACTIVE_WRITE);
        // non-MBR block, copy to cache
        int32_t dest = convert_block_to_flash_addr(block);
        if (dest == -1) {
            // bad block number
            return false;
        }
        enum status_code error_code;
        // A block is formed by two rows of flash. We must erase each row
        // before we write back to it.
        do
        {
            error_code = nvm_erase_row(dest);
        } while (error_code == STATUS_BUSY);
        if (error_code != STATUS_OK) {
            return false;
        }
        do
        {
            error_code = nvm_erase_row(dest + NVMCTRL_ROW_SIZE);
        } while (error_code == STATUS_BUSY);
        if (error_code != STATUS_OK) {
            return false;
        }

        // A block is made up of multiple pages. Write each page
        // sequentially.
        for (int i = 0; i < FILESYSTEM_BLOCK_SIZE / NVMCTRL_PAGE_SIZE; i++) {
          do
          {
              error_code = nvm_write_buffer(dest + i * NVMCTRL_PAGE_SIZE,
                                    src + i * NVMCTRL_PAGE_SIZE,
                                    NVMCTRL_PAGE_SIZE);
          } while (error_code == STATUS_BUSY);
          if (error_code != STATUS_OK) {
              return false;
          }
        }
        clear_temp_status();
        #ifdef MICROPY_HW_LED_MSC
            port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
        #endif
        return true;
    }
}

mp_uint_t internal_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!internal_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t internal_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!internal_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.

// there is a singleton Flash object
STATIC const mp_obj_base_t internal_flash_obj = {&internal_flash_type};

STATIC mp_obj_t internal_flash_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&internal_flash_obj;
}

STATIC mp_obj_t internal_flash_obj_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = internal_flash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(internal_flash_obj_readblocks_obj, internal_flash_obj_readblocks);

STATIC mp_obj_t internal_flash_obj_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = internal_flash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(internal_flash_obj_writeblocks_obj, internal_flash_obj_writeblocks);

STATIC mp_obj_t internal_flash_obj_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: internal_flash_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: internal_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: internal_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(internal_flash_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(internal_flash_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(internal_flash_obj_ioctl_obj, internal_flash_obj_ioctl);

STATIC const mp_map_elem_t internal_flash_obj_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_readblocks), (mp_obj_t)&internal_flash_obj_readblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeblocks), (mp_obj_t)&internal_flash_obj_writeblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ioctl), (mp_obj_t)&internal_flash_obj_ioctl_obj },
};

STATIC MP_DEFINE_CONST_DICT(internal_flash_obj_locals_dict, internal_flash_obj_locals_dict_table);

const mp_obj_type_t internal_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_InternalFlash,
    .make_new = internal_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&internal_flash_obj_locals_dict,
};

void flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = (mp_obj_t)&internal_flash_obj_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&internal_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)internal_flash_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&internal_flash_obj_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&internal_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)internal_flash_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&internal_flash_obj_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&internal_flash_obj;
}
