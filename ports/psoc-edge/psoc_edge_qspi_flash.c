/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <string.h>

// micropython includes
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modpsocedge.h"
#include "mplogger.h"

// MTB includes
#include "cybsp.h"
#include "cy_pdl.h"
#include "mphalport.h"
#include "cycfg_qspi_memslot.h"
#include "mtb_serial_memory.h"

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (EXT_FLASH_SIZE)
#endif
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % EXT_FLASH_SECTOR_SIZE == 0, "Flash storage size must be a multiple of sector size");

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (EXT_FLASH_BASE)
#endif

/* Slot number of the memory to use */
#define MEM_SLOT_NUM                        (0U)
#define MEM_SLOT_DIVIDER                    (2U)
#define MEM_SLOT_MULTIPLIER                 (2U)

/* 100 MHz interface clock frequency */
#define QSPI_BUS_FREQUENCY_HZ               (100000000Ul)

uint8_t qspi_flash_init = 0;

mtb_serial_memory_t serial_memory_obj;
cy_stc_smif_mem_context_t smif_mem_context;
cy_stc_smif_mem_info_t smif_mem_info;

typedef struct _psoc_edge_qspi_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} psoc_edge_qspi_flash_obj_t;

// Static singleton object for default flash access
static psoc_edge_qspi_flash_obj_t psoc_edge_qspi_flash_obj = {
    .base = { &psoc_edge_qspi_flash_type },
    .flash_base = EXT_FLASH_BASE,
    .flash_size = EXT_FLASH_SIZE,
};

static mp_obj_t psoc_edge_qspi_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    /* Set-up serial memory. */
    cy_rslt_t result = CY_RSLT_SUCCESS;

    if (!qspi_flash_init) {
        result = mtb_serial_memory_setup(&serial_memory_obj,
            MTB_SERIAL_MEMORY_CHIP_SELECT_1,
            CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_config.base,
            CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_config.clock,
            &smif_mem_context,
            &smif_mem_info,
            &smif0BlockConfig);

        if (result == CY_RSLT_SUCCESS) {
            qspi_flash_init = true;
        } else {
            mplogger_print("psoc_edge_qspi_flash_make_new() failed with error code: 0x%08lx\n", result);
            mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("psoc_edge_qspi_flash_make_new() - QSPI flash init failed!\n"));
        }
    }

    // Parse arguments
    enum { ARG_start, ARG_len };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        // Default singleton object that accesses entire flash
        return MP_OBJ_FROM_PTR(&psoc_edge_qspi_flash_obj);
    }

    // Create new object with custom start/len
    psoc_edge_qspi_flash_obj_t *self = mp_obj_malloc(psoc_edge_qspi_flash_obj_t, &psoc_edge_qspi_flash_type);

    mp_int_t start = args[ARG_start].u_int;

    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % EXT_FLASH_SECTOR_SIZE == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'start' value"));
    }

    mp_int_t len = args[ARG_len].u_int;

    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % EXT_FLASH_SECTOR_SIZE == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'len' value"));
    }

    self->flash_base = MICROPY_HW_FLASH_STORAGE_BASE + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t psoc_edge_qspi_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    psoc_edge_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    uint32_t offset = self->flash_base + (block_num * EXT_FLASH_SECTOR_SIZE);
    cy_rslt_t result = mtb_serial_memory_read(&serial_memory_obj, offset, bufinfo.len, bufinfo.buf);

    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Read failed: 0x%08lx"), result);
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc_edge_qspi_flash_readblocks_obj, 3, 4, psoc_edge_qspi_flash_readblocks);

static mp_obj_t psoc_edge_qspi_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    psoc_edge_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    uint32_t offset = self->flash_base + (block_num * EXT_FLASH_SECTOR_SIZE);

    // Erase before write only for full block writes (n_args == 3)
    // MTB flash library requires erased flash
    if (n_args == 3) {
        uint32_t numSectors = bufinfo.len / EXT_FLASH_SECTOR_SIZE;

        for (uint32_t i = 0; i < numSectors; i++) {
            cy_rslt_t erase_result = mtb_serial_memory_erase(&serial_memory_obj, offset + (i * EXT_FLASH_SECTOR_SIZE), EXT_FLASH_SECTOR_SIZE);
            if (erase_result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Erase before write failed: 0x%08lx"), erase_result);
            }
        }
    } else {
        offset += mp_obj_get_int(args[3]);
    }

    cy_rslt_t result = mtb_serial_memory_write(&serial_memory_obj, offset, bufinfo.len, bufinfo.buf);

    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Write failed: 0x%08lx"), result);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc_edge_qspi_flash_writeblocks_obj, 3, 4, psoc_edge_qspi_flash_writeblocks);

static mp_obj_t psoc_edge_qspi_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    psoc_edge_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / EXT_FLASH_SECTOR_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(EXT_FLASH_SECTOR_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t block_num = mp_obj_get_int(arg_in);
            uint32_t offset = self->flash_base + (block_num * EXT_FLASH_SECTOR_SIZE);
            cy_rslt_t result = mtb_serial_memory_erase(&serial_memory_obj, offset, EXT_FLASH_SECTOR_SIZE);
            if (result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Erase failed: 0x%08lx"), result);
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(psoc_edge_qspi_flash_ioctl_obj, psoc_edge_qspi_flash_ioctl);

static const mp_rom_map_elem_t psoc_edge_qspi_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&psoc_edge_qspi_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&psoc_edge_qspi_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&psoc_edge_qspi_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(psoc_edge_qspi_flash_locals_dict, psoc_edge_qspi_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    psoc_edge_qspi_flash_type,
    MP_QSTR_QSPI_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, psoc_edge_qspi_flash_make_new,
    locals_dict, &psoc_edge_qspi_flash_locals_dict
    );
