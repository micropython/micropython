/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "supervisor/shared/rgb_led_status.h"

#include "nrf_nvmc.h"

#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#endif

// defined in linker
extern uint32_t __fatfs_flash_start_addr[];
extern uint32_t __fatfs_flash_length[];

#define NO_CACHE        0xffffffff
#define FL_PAGE_SZ      4096

uint8_t  _flash_cache[FL_PAGE_SZ] __attribute__((aligned(4)));
uint32_t _flash_page_addr = NO_CACHE;


/*------------------------------------------------------------------*/
/* Internal Flash API
 *------------------------------------------------------------------*/
static inline uint32_t lba2addr(uint32_t block) {
    return ((uint32_t)__fatfs_flash_start_addr) + block * FILESYSTEM_BLOCK_SIZE;
}

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
    return ((uint32_t) __fatfs_flash_length) / FILESYSTEM_BLOCK_SIZE ;
}

// TODO support flashing with SD enabled
void internal_flash_flush(void) {
    if (_flash_page_addr == NO_CACHE) return;

    // Skip if data is the same
    if (memcmp(_flash_cache, (void *)_flash_page_addr, FL_PAGE_SZ) != 0) {
//        _is_flashing = true;
        nrf_nvmc_page_erase(_flash_page_addr);
        nrf_nvmc_write_words(_flash_page_addr, (uint32_t *)_flash_cache, FL_PAGE_SZ / sizeof(uint32_t));
    }

    _flash_page_addr = NO_CACHE;
}

mp_uint_t internal_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    uint32_t src = lba2addr(block);
    memcpy(dest, (uint8_t*) src, FILESYSTEM_BLOCK_SIZE*num_blocks);
    return 0; // success
}

mp_uint_t internal_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {

#ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
#endif

    while (num_blocks) {
        uint32_t const addr      = lba2addr(lba);
        uint32_t const page_addr = addr & ~(FL_PAGE_SZ - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            internal_flash_flush();

            // writing previous cached data, skip current data until flashing is done
            // tinyusb stack will invoke write_block() with the same parameters later on
            //        if ( _is_flashing ) return;

            _flash_page_addr = page_addr;
            memcpy(_flash_cache, (void *)page_addr, FL_PAGE_SZ);
        }

        memcpy(_flash_cache + (addr & (FL_PAGE_SZ - 1)), src, count * FILESYSTEM_BLOCK_SIZE);

        // adjust for next run
        lba        += count;
        src        += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }

#ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
#endif

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

STATIC const mp_rom_map_elem_t internal_flash_obj_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&internal_flash_obj_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&internal_flash_obj_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&internal_flash_obj_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(internal_flash_obj_locals_dict, internal_flash_obj_locals_dict_table);

const mp_obj_type_t internal_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_InternalFlash,
    .make_new = internal_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&internal_flash_obj_locals_dict,
};

/*------------------------------------------------------------------*/
/* Flash API
 *------------------------------------------------------------------*/

void flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;

//    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = (mp_obj_t)&internal_flash_obj_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&internal_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)internal_flash_read_blocks; // native version

    vfs->writeblocks[0] = (mp_obj_t)&internal_flash_obj_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&internal_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)internal_flash_write_blocks; // native version

    vfs->u.ioctl[0] = (mp_obj_t)&internal_flash_obj_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&internal_flash_obj;
}

void flash_flush(void) {
    internal_flash_flush();
}
