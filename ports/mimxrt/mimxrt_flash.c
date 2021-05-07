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

#include <string.h>

#include "py/runtime.h"
#include "extmod/vfs.h"
#include "mimxrt_flash.h"
#include "hal/flash.h"

// BOARD_FLASH_SIZE is defined in mpconfigport.h
#define SECTOR_SIZE_BYTES (qspiflash_config.sectorSize)
#define PAGE_SIZE_BYTES 256
// #define PAGE_SIZE_BYTES (qspiflash_config.pageSize)

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES  (1408 * 1024)
#endif

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (BOARD_FLASH_SIZE - MICROPY_HW_FLASH_STORAGE_BYTES)
#endif

// static_assert(MICROPY_HW_FLASH_STORAGE_BYTES <= BOARD_FLASH_SIZE "MICROPY_HW_FLASH_STORAGE_BYTES too big");
// static_assert(MICROPY_HW_FLASH_STORAGE_BASE + MICROPY_HW_FLASH_STORAGE_BYTES <= BOARD_FLASH_SIZE, "MICROPY_HW_FLASH_STORAGE_BYTES too big");

typedef struct _mimxrt_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} mimxrt_flash_obj_t;

STATIC mimxrt_flash_obj_t mimxrt_flash_obj = {
    .base = { &mimxrt_flash_type },
    .flash_base = MICROPY_HW_FLASH_STORAGE_BASE,
    .flash_size = MICROPY_HW_FLASH_STORAGE_BYTES,
};

// flash_erase_block(erase_addr_bytes)
// erases the 4k sector starting at adddr

bool flash_erase_block(uint32_t erase_addr)__attribute__((section(".ram_code"))) ;
bool flash_erase_block(uint32_t erase_addr) {
      	SCB_CleanInvalidateDCache();
    	SCB_DisableDCache();
		uint32_t primask = __get_PRIMASK();
		__set_PRIMASK(1);
        // disable execute
        // MPU->RBAR = ARM_MPU_RBAR(11, 0x600a0000U);
        // MPU->RASR = ARM_MPU_RASR(1, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_8MB);
        // flexspi_nor_reset();
        flexspi_nor_flash_erase_sector(FLEXSPI, erase_addr);
        // enable execute
        // MPU->RBAR = ARM_MPU_RBAR(11, 0x600a0000U);
        // MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_8MB);             
		__set_PRIMASK(primask);
	    SCB_EnableDCache();
		return true;
}

// flash_write_block(flash_dest_addr_bytes, data_source, length_bytes)
// writes length_byte data to the destination address
// length is a multiple of the page size = 256
// the vfs driver takes care for erasing the sector if required

bool flash_write_block(uint32_t dest_addr, const uint8_t *src, uint32_t length) __attribute__((section(".ram_code"))) ;
bool flash_write_block(uint32_t dest_addr, const uint8_t *src, uint32_t length) {
      	SCB_CleanInvalidateDCache();
    	SCB_DisableDCache();
		uint32_t primask = __get_PRIMASK();
		__set_PRIMASK(1);
        // disable execute
        MPU->RBAR = ARM_MPU_RBAR(11, 0x600a0000U);
        MPU->RASR = ARM_MPU_RASR(11, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_8MB);         
        // flexspi_nor_reset();
        // write sector in pages of 256 bytes
        status_t status = kStatus_Success;
        for (int i = 0; i < length; i += PAGE_SIZE_BYTES) {
            status = flexspi_nor_flash_page_program(FLEXSPI, dest_addr + i, (uint32_t*)(src + i), PAGE_SIZE_BYTES);	
            if (status != kStatus_Success) {
                status = false;
                break;
            }
        }
        // enable execute
        MPU->RBAR = ARM_MPU_RBAR(7, 0x600a0000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_8MB);  
		__set_PRIMASK(primask);
	    SCB_EnableDCache();
		return status;
}


STATIC mp_obj_t mimxrt_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check args.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // Upload the custom flash confguration
    FLEXSPI_UpdateLUT(FLEXSPI, 0, 
        qspiflash_config.memConfig.lookupTable, 
        sizeof(qspiflash_config.memConfig.lookupTable) / sizeof(qspiflash_config.memConfig.lookupTable[0]));
    // flexspi_nor_enable_quad_mode(FLEXSPI);

    // Return singleton object.
    return MP_OBJ_FROM_PTR(&mimxrt_flash_obj);
}

STATIC mp_obj_t mimxrt_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        mp_printf(MP_PYTHON_PRINTER, "readblocks: nargs = %d, block = %d, offset = %d, len = %d\n",
                  n_args, mp_obj_get_int(args[1]), mp_obj_get_int(args[3]), bufinfo.len);
    } else {
        mp_printf(MP_PYTHON_PRINTER, "readblocks: nargs = %d, block = %d, len = %d\n",
                  n_args, mp_obj_get_int(args[1]), bufinfo.len);
    }
    uint32_t offset = mp_obj_get_int(args[1]) * SECTOR_SIZE_BYTES;
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(bufinfo.buf, (uint8_t *)(FlexSPI_AMBA_BASE + self->flash_base + offset), bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mimxrt_flash_readblocks_obj, 3, 4, mimxrt_flash_readblocks);

STATIC mp_obj_t mimxrt_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * SECTOR_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    status_t status = kStatus_Success;

    if (n_args == 4) {
        mp_printf(MP_PYTHON_PRINTER, "writeblocks: nargs = %d, block = %d, offset = %d, len = %d\n",
                  n_args, mp_obj_get_int(args[1]), mp_obj_get_int(args[3]), bufinfo.len);
    } else {
        mp_printf(MP_PYTHON_PRINTER, "writeblocks: nargs = %d, block = %d, len = %d\n",
                  n_args, mp_obj_get_int(args[1]), bufinfo.len);
    }
    if (n_args == 3) {
        status = flash_erase_block(self->flash_base + offset);
        // TODO check return value
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    if (status == kStatus_Success) {
        status = flash_write_block(self->flash_base + offset, bufinfo.buf, bufinfo.len);
    }

    return MP_OBJ_NEW_SMALL_INT(status != kStatus_Success);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mimxrt_flash_writeblocks_obj, 3, 4, mimxrt_flash_writeblocks);

STATIC mp_obj_t mimxrt_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mimxrt_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    status_t status;
    uint8_t vendor_id = 0;
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            flexspi_nor_get_vendor_id(FLEXSPI, &vendor_id);
            return MP_OBJ_NEW_SMALL_INT(vendor_id);
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
            mp_printf(MP_PYTHON_PRINTER, "erase sector: address=%x\n", self->flash_base + offset);
            status = flash_erase_block(self->flash_base + offset);
            mp_printf(MP_PYTHON_PRINTER, "Status = %d\n", status);
            return MP_OBJ_NEW_SMALL_INT(status != kStatus_Success);
        }
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mimxrt_flash_ioctl_obj, mimxrt_flash_ioctl);

STATIC const mp_rom_map_elem_t mimxrt_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&mimxrt_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&mimxrt_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mimxrt_flash_ioctl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mimxrt_flash_locals_dict, mimxrt_flash_locals_dict_table);

const mp_obj_type_t mimxrt_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = mimxrt_flash_make_new,
    .locals_dict = (mp_obj_dict_t *)&mimxrt_flash_locals_dict,
};
