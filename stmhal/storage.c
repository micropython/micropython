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

#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/fatfs/ff.h"
#include "extmod/fsusermount.h"

#include "systick.h"
#include "led.h"
#include "flash.h"
#include "storage.h"
#include "irq.h"

#if defined(STM32F405xx) || defined(STM32F407xx)

#define CACHE_MEM_START_ADDR (0x10000000) // CCM data RAM, 64k
#define FLASH_SECTOR_SIZE_MAX (0x10000) // 64k max, size of CCM
#define FLASH_MEM_SEG1_START_ADDR (0x08004000) // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (224) // sectors 1,2,3,4: 16k+16k+16k+64k=112k

// enable this to get an extra 64k of storage (uses the last sector of the flash)
#if 0
#define FLASH_MEM_SEG2_START_ADDR (0x080e0000) // sector 11
#define FLASH_MEM_SEG2_NUM_BLOCKS (128) // sector 11: 128k
#endif

#elif defined(STM32F401xE) || defined(STM32F411xE)

STATIC byte flash_cache_mem[0x4000] __attribute__((aligned(4))); // 16k
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x4000) // 16k max due to size of cache buffer
#define FLASH_MEM_SEG1_START_ADDR (0x08004000) // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (128) // sectors 1,2,3,4: 16k+16k+16k+16k(of 64k)=64k

#elif defined(STM32F429xx)

#define CACHE_MEM_START_ADDR (0x10000000) // CCM data RAM, 64k
#define FLASH_SECTOR_SIZE_MAX (0x10000) // 64k max, size of CCM
#define FLASH_MEM_SEG1_START_ADDR (0x08004000) // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (224) // sectors 1,2,3,4: 16k+16k+16k+64k=112k

#elif defined(STM32F439xx)

#define CACHE_MEM_START_ADDR (0x10000000) // CCM data RAM, 64k
#define FLASH_SECTOR_SIZE_MAX (0x10000) // 64k max, size of CCM
#define FLASH_MEM_SEG1_START_ADDR (0x08100000) // sector 12
#define FLASH_MEM_SEG1_NUM_BLOCKS (384) // sectors 12,13,14,15,16,17: 16k+16k+16k+16k+64k+64k(of 128k)=192k
#define FLASH_MEM_SEG2_START_ADDR (0x08140000) // sector 18
#define FLASH_MEM_SEG2_NUM_BLOCKS (128) // sector 18: 64k(of 128k)

#elif defined(STM32F746xx)

// The STM32F746 doesn't really have CCRAM, so we use the 64K DTCM for this.

#define CACHE_MEM_START_ADDR (0x20000000) // DTCM data RAM, 64k
#define FLASH_SECTOR_SIZE_MAX (0x08000) // 32k max
#define FLASH_MEM_SEG1_START_ADDR (0x08008000) // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (192) // sectors 1,2,3: 32k+32k+32=96k

#elif defined(STM32L476xx)

extern uint8_t _flash_fs_start;
extern uint8_t _flash_fs_end;

// The STM32L476 doesn't have CCRAM, so we use the 32K SRAM2 for this.
#define CACHE_MEM_START_ADDR (0x10000000)       // SRAM2 data RAM, 32k
#define FLASH_SECTOR_SIZE_MAX (0x00800)         // 2k max
#define FLASH_MEM_SEG1_START_ADDR ((long)&_flash_fs_start)
#define FLASH_MEM_SEG1_NUM_BLOCKS ((&_flash_fs_end - &_flash_fs_start) / 512)

#else
#error "no storage support for this MCU"
#endif

#if !defined(FLASH_MEM_SEG2_START_ADDR)
#define FLASH_MEM_SEG2_START_ADDR (0) // no second segment
#define FLASH_MEM_SEG2_NUM_BLOCKS (0) // no second segment
#endif

#define FLASH_PART1_START_BLOCK (0x100)
#define FLASH_PART1_NUM_BLOCKS (FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS)

#define FLASH_FLAG_DIRTY        (1)
#define FLASH_FLAG_FORCE_WRITE  (2)
#define FLASH_FLAG_ERASED       (4)
static bool flash_is_initialised = false;
static __IO uint8_t flash_flags = 0;
static uint32_t flash_cache_sector_id;
static uint32_t flash_cache_sector_start;
static uint32_t flash_cache_sector_size;
static uint32_t flash_tick_counter_last_write;

static void flash_cache_flush(void) {
    if (flash_flags & FLASH_FLAG_DIRTY) {
        flash_flags |= FLASH_FLAG_FORCE_WRITE;
        while (flash_flags & FLASH_FLAG_DIRTY) {
           NVIC->STIR = FLASH_IRQn;
        }
    }
}

static uint8_t *flash_cache_get_addr_for_write(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_sector_size > FLASH_SECTOR_SIZE_MAX) {
        flash_sector_size = FLASH_SECTOR_SIZE_MAX;
    }
    if (flash_cache_sector_id != flash_sector_id) {
        flash_cache_flush();
        memcpy((void*)CACHE_MEM_START_ADDR, (const void*)flash_sector_start, flash_sector_size);
        flash_cache_sector_id = flash_sector_id;
        flash_cache_sector_start = flash_sector_start;
        flash_cache_sector_size = flash_sector_size;
    }
    flash_flags |= FLASH_FLAG_DIRTY;
    led_state(PYB_LED_R1, 1); // indicate a dirty cache with LED on
    flash_tick_counter_last_write = HAL_GetTick();
    return (uint8_t*)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
}

static uint8_t *flash_cache_get_addr_for_read(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_cache_sector_id == flash_sector_id) {
        // in cache, copy from there
        return (uint8_t*)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
    }
    // not in cache, copy straight from flash
    return (uint8_t*)flash_addr;
}

void storage_init(void) {
    if (!flash_is_initialised) {
        flash_flags = 0;
        flash_cache_sector_id = 0;
        flash_tick_counter_last_write = 0;
        flash_is_initialised = true;
    }

    // Enable the flash IRQ, which is used to also call our storage IRQ handler
    // It needs to go at a higher priority than all those components that rely on
    // the flash storage (eg higher than USB MSC).
    HAL_NVIC_SetPriority(FLASH_IRQn, IRQ_PRI_FLASH, IRQ_SUBPRI_FLASH);
    HAL_NVIC_EnableIRQ(FLASH_IRQn);
}

uint32_t storage_get_block_size(void) {
    return FLASH_BLOCK_SIZE;
}

uint32_t storage_get_block_count(void) {
    return FLASH_PART1_START_BLOCK + FLASH_PART1_NUM_BLOCKS;
}

void storage_irq_handler(void) {
    if (!(flash_flags & FLASH_FLAG_DIRTY)) {
        return;
    }

    // This code uses interrupts to erase the flash
    /*
    if (flash_erase_state == 0) {
        flash_erase_it(flash_cache_sector_start, (const uint32_t*)CACHE_MEM_START_ADDR, flash_cache_sector_size / 4);
        flash_erase_state = 1;
        return;
    }

    if (flash_erase_state == 1) {
        // wait for erase
        // TODO add timeout
        #define flash_erase_done() (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) == RESET)
        if (!flash_erase_done()) {
            return;
        }
        flash_erase_state = 2;
    }
    */

    // This code erases the flash directly, waiting for it to finish
    if (!(flash_flags & FLASH_FLAG_ERASED)) {
        flash_erase(flash_cache_sector_start, (const uint32_t*)CACHE_MEM_START_ADDR, flash_cache_sector_size / 4);
        flash_flags |= FLASH_FLAG_ERASED;
        return;
    }

    // If not a forced write, wait at least 5 seconds after last write to flush
    // On file close and flash unmount we get a forced write, so we can afford to wait a while
    if ((flash_flags & FLASH_FLAG_FORCE_WRITE) || sys_tick_has_passed(flash_tick_counter_last_write, 5000)) {
        // sync the cache RAM buffer by writing it to the flash page
        flash_write(flash_cache_sector_start, (const uint32_t*)CACHE_MEM_START_ADDR, flash_cache_sector_size / 4);
        // clear the flash flags now that we have a clean cache
        flash_flags = 0;
        // indicate a clean cache with LED off
        led_state(PYB_LED_R1, 0);
    }
}

void storage_flush(void) {
    flash_cache_flush();
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

static uint32_t convert_block_to_flash_addr(uint32_t block) {
    if (FLASH_PART1_START_BLOCK <= block && block < FLASH_PART1_START_BLOCK + FLASH_PART1_NUM_BLOCKS) {
        // a block in partition 1
        block -= FLASH_PART1_START_BLOCK;
        if (block < FLASH_MEM_SEG1_NUM_BLOCKS) {
            return FLASH_MEM_SEG1_START_ADDR + block * FLASH_BLOCK_SIZE;
        } else if (block < FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS) {
            return FLASH_MEM_SEG2_START_ADDR + (block - FLASH_MEM_SEG1_NUM_BLOCKS) * FLASH_BLOCK_SIZE;
        }
        // can add more flash segments here if needed, following above pattern
    }
    // bad block
    return -1;
}

bool storage_read_block(uint8_t *dest, uint32_t block) {
    //printf("RD %u\n", block);
    if (block == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, FLASH_PART1_START_BLOCK, FLASH_PART1_NUM_BLOCKS);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;

    } else {
        // non-MBR block, get data from flash memory, possibly via cache
        uint32_t flash_addr = convert_block_to_flash_addr(block);
        if (flash_addr == -1) {
            // bad block number
            return false;
        }
        uint8_t *src = flash_cache_get_addr_for_read(flash_addr);
        memcpy(dest, src, FLASH_BLOCK_SIZE);
        return true;
    }
}

bool storage_write_block(const uint8_t *src, uint32_t block) {
    //printf("WR %u\n", block);
    if (block == 0) {
        // can't write MBR, but pretend we did
        return true;

    } else {
        // non-MBR block, copy to cache
        uint32_t flash_addr = convert_block_to_flash_addr(block);
        if (flash_addr == -1) {
            // bad block number
            return false;
        }
        uint8_t *dest = flash_cache_get_addr_for_write(flash_addr);
        memcpy(dest, src, FLASH_BLOCK_SIZE);
        return true;
    }
}

mp_uint_t storage_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!storage_read_block(dest + i * FLASH_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t storage_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!storage_write_block(src + i * FLASH_BLOCK_SIZE, block_num + i)) {
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
STATIC const mp_obj_base_t pyb_flash_obj = {&pyb_flash_type};

STATIC mp_obj_t pyb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&pyb_flash_obj;
}

STATIC mp_obj_t pyb_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = storage_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_readblocks_obj, pyb_flash_readblocks);

STATIC mp_obj_t pyb_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = storage_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_writeblocks_obj, pyb_flash_writeblocks);

STATIC mp_obj_t pyb_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: storage_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: storage_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: storage_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(storage_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(storage_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_ioctl_obj, pyb_flash_ioctl);

STATIC const mp_map_elem_t pyb_flash_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_readblocks), (mp_obj_t)&pyb_flash_readblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeblocks), (mp_obj_t)&pyb_flash_writeblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ioctl), (mp_obj_t)&pyb_flash_ioctl_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_flash_locals_dict, pyb_flash_locals_dict_table);

const mp_obj_type_t pyb_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = pyb_flash_make_new,
    .locals_dict = (mp_obj_t)&pyb_flash_locals_dict,
};

void pyb_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->readblocks[0] = (mp_obj_t)&pyb_flash_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)storage_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&pyb_flash_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)storage_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&pyb_flash_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&pyb_flash_obj;
}
