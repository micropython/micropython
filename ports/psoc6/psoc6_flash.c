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
#include "modpsoc6.h"
#include "mplogger.h"
#include "mphalport.h"


// MTB includes
#include "cyhal.h"

#if defined(CY8C624ABZI_S2D44)
#define DEVICE_FLASH_SIZE           (0x00200000)   // 2MB
#define DEVICE_FLASH_BASE_ADDRESS   (0x101A0000)   // Where is this value coming from??

#elif defined(CYBLE_416045_02_device)
#define DEVICE_FLASH_SIZE           (0x00100000)   // 1MB
#define DEVICE_FLASH_BASE_ADDRESS   (0x100E0000)   // Where is this value coming from??

#else
#error "MCU not supported."
#endif


#define FLASH_BASE_TRUE   (0x10000000)

#define FLASH_BASE        DEVICE_FLASH_BASE_ADDRESS
#define FLASH_SIZE        (DEVICE_FLASH_SIZE - (FLASH_BASE - FLASH_BASE_TRUE))

#define FLASH_SECTOR_SIZE (0x200)
#define BLOCK_SIZE_BYTES  (FLASH_SECTOR_SIZE)

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (FLASH_SIZE)
#endif
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % 4096 == 0, "Flash storage size must be a multiple of 4K");

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (FLASH_BASE)
#endif


typedef struct _psoc6_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} psoc6_flash_obj_t;

static psoc6_flash_obj_t psoc6_flash_obj = {
    .base = { &psoc6_flash_type },
    .flash_base = FLASH_BASE,
    .flash_size = FLASH_SIZE,
};

cyhal_flash_t cyhal_flash_obj;
cyhal_flash_info_t flash_info;

// Helper function to get internal flash configurations
void get_flash_info(void) {
    mplogger_print("\nRetrieving internal flash info...\n");
    cyhal_flash_get_info(&cyhal_flash_obj, &flash_info);
    /* Wait for 100ms for the flash write to complete */
    uint32_t timeout = 100;
    /* Wait for the command to finish execution */
    while ((true != cyhal_flash_is_operation_complete(&cyhal_flash_obj)) && (0 < timeout)) {
        timeout--;
        cyhal_system_delay_ms(1); /* delay one millisecond each iteration */
    }
    uint32_t total_flash_size = 0;
    uint32_t page_size = 0;
    if (0 != timeout) {
        for (int index = 0; index < flash_info.block_count; index++)
        {
            const cyhal_flash_block_info_t *block_info = flash_info.blocks;
            total_flash_size += block_info->size;
            page_size = block_info->page_size;
        }
    }

    mplogger_print("\nTotal flash size (MB): %ld\n", total_flash_size / (1024 * 1024));
    mplogger_print("\nTotal no. of blocks: %d\n", flash_info.block_count);
    mplogger_print("\nPage size (bytes): %ld\n", page_size);
}

static mp_obj_t psoc6_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mplogger_print("\nFlash constructor invoked\n");
    #if MICROPY_LOGGER_DEBUG
    get_flash_info();
    #endif

    cy_rslt_t result = CY_RSLT_SUCCESS;

    result = cyhal_flash_init(&cyhal_flash_obj);
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_flash_make_new() failed while initializing flash with error code : %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("psoc6_flash_make_new() - QSPI flash init failed !\n"));
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
        return MP_OBJ_FROM_PTR(&psoc6_flash_obj);
    }

    psoc6_flash_obj_t *self = mp_obj_malloc(psoc6_flash_obj_t, &psoc6_flash_type);

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

static mp_obj_t psoc6_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("\nFlash readblocks called\n");

    psoc6_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }

    cy_rslt_t result = cyhal_flash_read(&cyhal_flash_obj, self->flash_base + offset, bufinfo.buf, bufinfo.len);
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_flash_readblocks() failed while reading the flash with error code: %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_ValueError(MP_ERROR_TEXT("psoc6_flash_readblocks() - Flash Read failed !"));
    }

    // TODO: or simply do it like this ?
    // memcpy(bufinfo.buf, (void *)(self->flash_base + offset), bufinfo.len);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_flash_readblocks_obj, 3, 4, psoc6_flash_readblocks);

static mp_obj_t psoc6_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    mplogger_print("\nFlash writeblocks called\n");
    psoc6_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    if (n_args == 3) {
        uint32_t numSectors = bufinfo.len / FLASH_SECTOR_SIZE;

        for (uint32_t i = 0; i <= numSectors; ++i) {
            cy_rslt_t result = cyhal_flash_erase(&cyhal_flash_obj, self->flash_base + offset + i * FLASH_SECTOR_SIZE);

            if (CY_RSLT_SUCCESS != result) {
                mplogger_print("\npsoc6_flash_writeblocks() failed while erasing the flash with error code: %u\n", CY_RSLT_GET_CODE(result));
                mp_raise_ValueError(MP_ERROR_TEXT("psoc6_flash_writeblocks() - Flash Erase failed !"));
            }
        }
    } else {
        offset += mp_obj_get_int(args[3]);
    }


    // Flash erase/program must run in an atomic section.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    cy_rslt_t result = cyhal_flash_write(&cyhal_flash_obj, self->flash_base + offset, bufinfo.buf);
    if (CY_RSLT_SUCCESS != result) {
        mplogger_print("psoc6_flash_writeblocks() failed while writing with error code: %u\n", CY_RSLT_GET_CODE(result));
        mp_raise_ValueError(MP_ERROR_TEXT("psoc6_flash_writeblocks() - Flash Write failed!"));
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(psoc6_flash_writeblocks_obj, 3, 4, psoc6_flash_writeblocks);

static mp_obj_t psoc6_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mplogger_print("Flash ioctrl called\n");
    psoc6_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
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
            mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
            cy_rslt_t result = cyhal_flash_erase(&cyhal_flash_obj, self->flash_base + offset);

            if (CY_RSLT_SUCCESS != result) {
                mplogger_print("psoc6_flash_ioctl() failed while erasing block with error code: %u\n", CY_RSLT_GET_CODE(result));
                mp_raise_ValueError(MP_ERROR_TEXT("psoc6_flash_ioctl() - Flash erase failed !"));
            }

            MICROPY_END_ATOMIC_SECTION(atomic_state);
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(psoc6_flash_ioctl_obj, psoc6_flash_ioctl);

static const mp_rom_map_elem_t psoc6_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&psoc6_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&psoc6_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&psoc6_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(psoc6_flash_locals_dict, psoc6_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    psoc6_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, psoc6_flash_make_new,
    locals_dict, &psoc6_flash_locals_dict
    );
