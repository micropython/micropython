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

#define FLASH_BASE        (0x00)  // absolute address of xip/qspi flash. Does not depend on ld file values since xip isn't used
#define FLASH_SIZE        (0x4000000)  // qspi flash is 512 Mb / 64 MB in size

#define FLASH_SECTOR_SIZE (0x40000) // 256 KB sector size uniform (depends on cfg register). See Table 46 of https://www.infineon.com/dgdl/Infineon-S25HS256T_S25HS512T_S25HS01GT_S25HL256T_S25HL512T_S25HL01GT_256-Mb_(32-MB)_512-Mb_(64-MB)_1-Gb_(128-MB)_HS-T_(1.8-V)_HL-T_(3.0-V)_Semper_Flash_with_Quad_SPI-DataSheet-v02_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee674b86ee3&da=t
// However, tests reveal that all sectors are in fact 256 KB deep. The factory settings must have been overwritten at some point.
// See pg. 22 of above datasheet for address maps.
#define BLOCK_SIZE_BYTES  (FLASH_SECTOR_SIZE) // TODO: verify, since initial 32 sectors are only 4 KB in size
// TODO: edit cfg register contents to make all sectors uniform, see pg 25 of https://www.infineon.com/dgdl/Infineon-S25HS256T_S25HS512T_S25HS01GT_S25HL256T_S25HL512T_S25HL01GT_256-Mb_(32-MB)_512-Mb_(64-MB)_1-Gb_(128-MB)_HS-T_(1.8-V)_HL-T_(3.0-V)_Semper_Flash_with_Quad_SPI-DataSheet-v02_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee674b86ee3&da=t
#define FLASH_PAGE_SIZE (0x200) // qspi flash page size is 256 B (Factory default), see pg. 80 of https://www.infineon.com/dgdl/Infineon-S25HS256T_S25HS512T_S25HS01GT_S25HL256T_S25HL512T_S25HL01GT_256-Mb_(32-MB)_512-Mb_(64-MB)_1-Gb_(128-MB)_HS-T_(1.8-V)_HL-T_(3.0-V)_Semper_Flash_with_Quad_SPI-DataSheet-v02_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee674b86ee3&da=t
// but config says it is 512 B

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (FLASH_SIZE)
#endif
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % 4096 == 0, "Flash storage size must be a multiple of 4K");

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (FLASH_BASE)
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
    .flash_base = FLASH_BASE,
    .flash_size = FLASH_SIZE,
};


// function to erase the entire flash
void cy_erase_entire_flash(void) {
    mp_printf(&mp_plat_print, "\nErasing entire flash... might take a while");
    cy_serial_flash_qspi_erase(0, cy_serial_flash_qspi_get_size());
    mp_printf(&mp_plat_print, "\nDone");
}


STATIC mp_obj_t psoc6_qspi_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mplogger_print("qspi flash constructor invoked\n");

    // variable initialized to capture status
    cy_rslt_t result = CY_RSLT_SUCCESS;
    // init only if not init before
    if (!qspi_flash_init) {
        /* Initialize the QSPI block */
        result = cy_serial_flash_qspi_init(smifMemConfigs[MEM_SLOT_NUM], CYBSP_QSPI_D0, CYBSP_QSPI_D1,
            CYBSP_QSPI_D2, CYBSP_QSPI_D3, NC, NC, NC, NC, CYBSP_QSPI_SCK,
            CYBSP_QSPI_SS, QSPI_BUS_FREQUENCY_HZ);
        qspi_flash_init = 1;
    }
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("Error code: %u\n", CY_RSLT_GET_CODE(result)); // see file cy_result.h to decode result values
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("QSPI Flash INIT failed !\n"));
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
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'start' value specified for psoc6_flash_make_new !\n"));
    }

    mp_int_t len = args[ARG_len].u_int;

    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid 'len' value specified for psoc6_flash_make_new !\n"));
    }

    self->flash_base = MICROPY_HW_FLASH_STORAGE_BASE + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t psoc6_qspi_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("qspi flash readblocks called\n");
    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    mplogger_print("Address in hex:%04X, Length:%u\n", self->flash_base + offset, bufinfo.len);

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION(); // begin atomic section
    cy_rslt_t result = cy_serial_flash_qspi_read(self->flash_base + offset, bufinfo.len, bufinfo.buf);

    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("Error code: %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_ValueError(MP_ERROR_TEXT("QSPI Flash Read failed !"));
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state); // end atomic section
    ;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_qspi_flash_readblocks_obj, 3, 4, psoc6_qspi_flash_readblocks);

STATIC mp_obj_t psoc6_qspi_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("qspi flash writeblocks called\n");
    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    if (n_args == 3) {
        // Flash erase/program must run in an atomic section.
        // TODO: replace with implemented machine.disable_irq() in machine module.
        // TODO: that function implements the atomic_state hash logic.
        // TODO: that function must be made non-static first.
        // TODO: or, reimplement MICROPY_BEGIN_ATOMIC_SECTION() as a simple macro
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION(); // begin atomic section
        uint32_t numSectors = bufinfo.len / FLASH_SECTOR_SIZE;

        for (uint32_t i = 0; i <= numSectors; ++i) {
            mplogger_print("Address in hex:%04X\n", self->flash_base + offset + i * FLASH_SECTOR_SIZE);
            cy_rslt_t result = cy_serial_flash_qspi_erase(self->flash_base + offset + i * FLASH_SECTOR_SIZE, cy_serial_flash_qspi_get_erase_size(self->flash_base + offset + i * FLASH_SECTOR_SIZE));
            // the cy_serial_flash_qspi_get_erase_size() function call is necessary to keep the erase at sector boundary, else it throws errors.

            if (CY_RSLT_SUCCESS != result) {
                mplogger_print("Error code: %u\n", CY_RSLT_GET_CODE(result));
                mp_raise_ValueError(MP_ERROR_TEXT("QSPI Flash Erase failed !"));
            }
        }

        MICROPY_END_ATOMIC_SECTION(atomic_state); // end atomic section
    } else {
        offset += mp_obj_get_int(args[3]);
    }


    // Flash erase/program must run in an atomic section.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t numPages = bufinfo.len / FLASH_PAGE_SIZE;

    for (uint32_t i = 0; i <= numPages; ++i) {
        mplogger_print("Address in hex:%04X\n", self->flash_base + offset + i * FLASH_PAGE_SIZE);
        cy_rslt_t result = cy_serial_flash_qspi_write(self->flash_base + offset + i * FLASH_PAGE_SIZE, FLASH_PAGE_SIZE, bufinfo.buf + i * FLASH_PAGE_SIZE);  // TODO: verify this

        if (CY_RSLT_SUCCESS != result) {
            mplogger_print("Error code: %u\n", CY_RSLT_GET_CODE(result));
            mp_raise_ValueError(MP_ERROR_TEXT("QSPI Flash Write failed !")); // TODO: replace with proper exception message. also few instances above and below.
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_qspi_flash_writeblocks_obj, 3, 4, psoc6_qspi_flash_writeblocks);

STATIC mp_obj_t psoc6_qspi_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mplogger_print("qspi flash ioctrl called\n");
    psoc6_qspi_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    mplogger_print("option:%u\n", cmd);

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
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * BLOCK_SIZE_BYTES;
            // Flash erase/program must run in an atomic section.
            mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION(); // begin atomic section
            cy_rslt_t result = cy_serial_flash_qspi_erase(self->flash_base + offset, cy_serial_flash_qspi_get_erase_size(self->flash_base + offset));

            if (CY_RSLT_SUCCESS != result) {
                mp_raise_ValueError(MP_ERROR_TEXT("QSPI Flash Erase failed !"));
            }
            MICROPY_END_ATOMIC_SECTION(atomic_state); // end atomic section
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
