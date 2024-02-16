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

// std includes
#include <stdio.h>
#include <string.h>

// micropython includes
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modpsoc6.h"
#include "mplogger.h"

// MTB includes
#include "cyhal.h"
#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_serial_flash_qspi.h"
#include "cycfg_qspi_memslot.h"
#include "mphalport.h"


#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (EXT_FLASH_SIZE)
#endif
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % 4096 == 0, "Flash storage size must be a multiple of 4K");

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (EXT_FLASH_BASE)
#endif

// CY Macros
#define MEM_SLOT_NUM            (0u)      /* Slot number of the memory to use - For CYPROTO 062 4343W there is one slave slot for QSPI NOR FLASH CHIP*/
#define QSPI_BUS_FREQUENCY_HZ   (50000000lu) /* Running freq of qspi bus = 50 MHz */

// flag to set of spi flash is init'd
uint8_t qspi_flash_init = 0;

typedef struct _psoc6_qspi_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} psoc6_qspi_flash_obj_t;

STATIC psoc6_qspi_flash_obj_t psoc6_qspi_flash_obj = {
    .base = { &psoc6_qspi_flash_type },
    .flash_base = EXT_FLASH_BASE,
    .flash_size = EXT_FLASH_SIZE,
};


// function to erase the entire flash
void cy_erase_entire_flash(void) {
    mp_printf(&mp_plat_print, "\nErasing entire flash... might take a while\n");
    cy_serial_flash_qspi_erase(0, cy_serial_flash_qspi_get_size());
    mp_printf(&mp_plat_print, "\nDone\n");
}

// Helper function to get external flash configurations
void get_ext_flash_info(void) {
    mplogger_print("\nRetrieving external flash info...\n");
    mplogger_print("\nTotal flash size (MB): %d\n", cy_serial_flash_qspi_get_size() / (1024 * 1024));
    mplogger_print("\nSize of erase sector (bytes): %d\n", cy_serial_flash_qspi_get_erase_size(0x00) / (1024));
    mplogger_print("\nPage size (bytes): %d\n", cy_serial_flash_qspi_get_prog_size(0x00));
}

STATIC mp_obj_t psoc6_qspi_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mplogger_print("\nQSPI flash constructor invoked\n");
    get_ext_flash_info();

    cy_rslt_t result = CY_RSLT_SUCCESS;

    if (!qspi_flash_init) {
        result = cy_serial_flash_qspi_init(smifMemConfigs[MEM_SLOT_NUM], CYBSP_QSPI_D0, CYBSP_QSPI_D1,
            CYBSP_QSPI_D2, CYBSP_QSPI_D3, NC, NC, NC, NC, CYBSP_QSPI_SCK,
            CYBSP_QSPI_SS, QSPI_BUS_FREQUENCY_HZ);
        qspi_flash_init = 1;
    }
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_qspi_flash_make_new() failed while initializing flash with error code : %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("psoc6_qspi_flash_make_new() - QSPI flash init failed !\n"));
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
        return MP_OBJ_FROM_PTR(&psoc6_qspi_flash_obj);
    }

    psoc6_qspi_flash_obj_t *self = mp_obj_malloc(psoc6_qspi_flash_obj_t, &psoc6_qspi_flash_type);

    mp_int_t start = args[ARG_start].u_int;

    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % EXT_FLASH_BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'start' value specified for psoc6_flash_make_new !\n"));
    }

    mp_int_t len = args[ARG_len].u_int;

    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % EXT_FLASH_BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'len' value specified for psoc6_flash_make_new !\n"));
    }

    self->flash_base = MICROPY_HW_FLASH_STORAGE_BASE + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t psoc6_qspi_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("\nQSPI flash readblocks called\n");

    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * EXT_FLASH_BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    cy_rslt_t result = cy_serial_flash_qspi_read(self->flash_base + offset, bufinfo.len, bufinfo.buf);

    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_qspi_flash_readblocks() failed while reading the flash with error code: %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_ValueError(MP_ERROR_TEXT("psoc6_qspi_flash_readblocks() - QSPI Flash Read failed !"));
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    ;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_qspi_flash_readblocks_obj, 3, 4, psoc6_qspi_flash_readblocks);

STATIC mp_obj_t psoc6_qspi_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("\nQSPI flash writeblocks called\n");
    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * EXT_FLASH_BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    if (n_args == 3) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        uint32_t numSectors = bufinfo.len / EXT_FLASH_SECTOR_SIZE;

        for (uint32_t i = 0; i <= numSectors; ++i) {
            cy_rslt_t result = cy_serial_flash_qspi_erase(self->flash_base + offset + i * EXT_FLASH_SECTOR_SIZE, cy_serial_flash_qspi_get_erase_size(self->flash_base + offset + i * EXT_FLASH_SECTOR_SIZE));
            // the cy_serial_flash_qspi_get_erase_size() function call is necessary to keep the erase at sector boundary, else it throws errors.

            if (CY_RSLT_SUCCESS != result) {
                mplogger_print("\npsoc6_qspi_flash_writeblocks() failed while erasing the flash with error code: %u\n", CY_RSLT_GET_CODE(result));
                mp_raise_ValueError(MP_ERROR_TEXT("psoc6_qspi_flash_writeblocks() - QSPI flash Erase failed !"));
            }
        }

        MICROPY_END_ATOMIC_SECTION(atomic_state);
    } else {
        offset += mp_obj_get_int(args[3]);
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    cy_rslt_t result = cy_serial_flash_qspi_write(self->flash_base + offset, bufinfo.len, bufinfo.buf);
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_qspi_flash_writeblocks() failed while writing with error code: %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_ValueError(MP_ERROR_TEXT("psoc6_qspi_flash_writeblocks() - QSPI Flash Write failed!"));
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_qspi_flash_writeblocks_obj, 3, 4, psoc6_qspi_flash_writeblocks);

STATIC mp_obj_t psoc6_qspi_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mplogger_print("QSPI flash ioctrl called\n");
    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            if (!qspi_flash_init) {
                /* Initialize the QSPI block */
                cy_serial_flash_qspi_init(smifMemConfigs[MEM_SLOT_NUM], CYBSP_QSPI_D0, CYBSP_QSPI_D1,
                    CYBSP_QSPI_D2, CYBSP_QSPI_D3, NC, NC, NC, NC, CYBSP_QSPI_SCK,
                    CYBSP_QSPI_SS, QSPI_BUS_FREQUENCY_HZ);
                qspi_flash_init = 1;
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            if (qspi_flash_init) {
                cy_serial_flash_qspi_deinit();
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / EXT_FLASH_BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(EXT_FLASH_BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * EXT_FLASH_BLOCK_SIZE_BYTES;
            cy_rslt_t result = cy_serial_flash_qspi_erase(self->flash_base + offset, cy_serial_flash_qspi_get_erase_size(self->flash_base + offset));

            if (CY_RSLT_SUCCESS != result) {
                mplogger_print("psoc6_qspi_flash_ioctl() failed while erasing block with error code: %u\n", CY_RSLT_GET_CODE(result));
                mp_raise_ValueError(MP_ERROR_TEXT("psoc6_qspi_flash_ioctl() - QSPI Flash erase failed !"));
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(psoc6_qspi_flash_ioctl_obj, psoc6_qspi_flash_ioctl);

STATIC const mp_rom_map_elem_t psoc6_qspi_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&psoc6_qspi_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&psoc6_qspi_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&psoc6_qspi_flash_ioctl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(psoc6_qspi_flash_locals_dict, psoc6_qspi_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    psoc6_qspi_flash_type,
    MP_QSTR_QSPI_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, psoc6_qspi_flash_make_new,
    locals_dict, &psoc6_qspi_flash_locals_dict
    );
