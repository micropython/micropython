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
#include <string.h>

// micropython includes
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modpsoc6.h"
#include "mplogger.h"
#include "mphalport.h"

// port-specific includes
#include "modmachine.h"
#include "machine_pin_phy.h"
#include "mplogger.h"

// MTB includes
#include "cyhal.h"
#include "cybsp.h"

#define SDHC_DEFAULT_BUS_WIDTH                  (4U)
#define SDHC_BLOCK_SIZE                         (512UL)
#define SDHC_CLOCK_RESOURCE_BLOCK_NUMBER        (18U)
#define SDHC_CLOCK_RESOURCE_CHANNEL_NUMBER      (2U)
typedef struct _machine_sdcard_obj_t {
    mp_obj_base_t base;
    cyhal_sdhc_t sdhc_obj;
    uint16_t slot_num;
    machine_pin_phy_obj_t *wp;
    machine_pin_phy_obj_t *cmd;
    machine_pin_phy_obj_t *dat0;
    machine_pin_phy_obj_t *dat1;
    machine_pin_phy_obj_t *dat2;
    machine_pin_phy_obj_t *dat3;
    machine_pin_phy_obj_t *clk;
    machine_pin_phy_obj_t *cd;
    uint32_t block_count;
} machine_sdcard_obj_t;

machine_sdcard_obj_t *sdhc_obj[MAX_SDHC_SLOT] = { NULL };

enum {
    ARG_slot,
    ARG_width,
    ARG_cd,
    ARG_wp,
    ARG_cmd,
    ARG_dat0,
    ARG_dat1,
    ARG_dat2,
    ARG_dat3,
    ARG_clk
};

static inline machine_sdcard_obj_t *sd_card_obj_alloc() {
    for (uint8_t i = 0; i < MAX_SDHC_SLOT; i++)
    {
        if (sdhc_obj[i] == NULL) {
            sdhc_obj[i] = mp_obj_malloc(machine_sdcard_obj_t, &machine_sdcard_type);
            return sdhc_obj[i];
        }
    }
    return NULL;
}

static inline void sd_card_obj_free(machine_sdcard_obj_t *sdhc_obj_ptr) {
    for (uint8_t i = 0; i < MAX_SDHC_SLOT; i++)
    {
        if (sdhc_obj[i] == sdhc_obj_ptr) {
            sdhc_obj[i] = NULL;
        }
    }
}

static void sd_card_allocate_pin(machine_sdcard_obj_t *self, mp_arg_val_t *args) {
    if (args[ARG_cmd].u_obj != mp_const_none) {
        self->cmd = pin_phy_realloc(args[ARG_cmd].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Cmd pin must be provided"));
    }

    if (args[ARG_dat0].u_obj != mp_const_none) {
        self->dat0 = pin_phy_realloc(args[ARG_dat0].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Data 0 pin must be provided"));
    }

    if (args[ARG_dat1].u_obj != mp_const_none) {
        self->dat1 = pin_phy_realloc(args[ARG_dat1].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Data 1 pin must be provided"));
    }

    if (args[ARG_dat2].u_obj != mp_const_none) {
        self->dat2 = pin_phy_realloc(args[ARG_dat2].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Data 2 pin must be provided"));
    }

    if (args[ARG_dat3].u_obj != mp_const_none) {
        self->dat3 = pin_phy_realloc(args[ARG_dat3].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Data 3 pin must be provided"));
    }

    if (args[ARG_clk].u_obj != mp_const_none) {
        self->clk = pin_phy_realloc(args[ARG_clk].u_obj, PIN_PHY_FUNC_SDHC);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("Clk pin must be provided"));
    }

    if (args[ARG_cd].u_obj != mp_const_none) {
        self->cd = pin_phy_realloc(args[ARG_cd].u_obj, PIN_PHY_FUNC_SDHC);
    }

    if (args[ARG_wp].u_obj != mp_const_none) {
        self->wp = pin_phy_realloc(args[ARG_wp].u_obj, PIN_PHY_FUNC_SDHC);
    }
}

static cy_rslt_t sd_card_init_helper(machine_sdcard_obj_t *self, mp_arg_val_t *args) {

    cyhal_sdhc_config_t sdhc_config;

    sdhc_config.enableLedControl = false;
    sdhc_config.lowVoltageSignaling = false;
    sdhc_config.isEmmc = false;
    sdhc_config.busWidth = SDHC_DEFAULT_BUS_WIDTH;

    cy_rslt_t result = CY_RSLT_SUCCESS;
    cyhal_clock_t *clock_source = NULL;

// If external flash is running, then HF2 clock resource is already initialized by QSPI and it has to be reused by SDHC peripheral.
    #if MICROPY_ENABLE_EXT_QSPI_FLASH
    cyhal_clock_t clock;
    cyhal_resource_inst_t rsc = {CYHAL_RSC_CLOCK, SDHC_CLOCK_RESOURCE_BLOCK_NUMBER, SDHC_CLOCK_RESOURCE_CHANNEL_NUMBER};
    cyhal_clock_get(&clock, &rsc);
    clock_source = &clock;
    #endif


    sd_card_allocate_pin(self, args);
    result = cyhal_sdhc_init(&self->sdhc_obj, &sdhc_config, self->cmd->addr, self->clk->addr, self->dat0->addr, self->dat1->addr,
        self->dat2->addr, self->dat3->addr, NC, NC, NC, NC, self->cd->addr, NC, NC, NC, NC, NC, clock_source);
    return result;
}

static void sd_card_deallocate_pins(machine_sdcard_obj_t *self) {
    cyhal_gpio_free(self->cd->addr);
    cyhal_gpio_free(self->cmd->addr);
    cyhal_gpio_free(self->clk->addr);
    cyhal_gpio_free(self->wp->addr);
    cyhal_gpio_free(self->dat0->addr);
    cyhal_gpio_free(self->dat1->addr);
    cyhal_gpio_free(self->dat2->addr);
    cyhal_gpio_free(self->dat3->addr);
}

static mp_obj_t machine_sdcard_deinit(mp_obj_t self_in) {
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_sdhc_free(&self->sdhc_obj);
    sd_card_deallocate_pins(self);
    sd_card_obj_free(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_deinit_obj, machine_sdcard_deinit);

static mp_obj_t machine_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    mp_arg_check_num(n_args, n_kw, 0, 9, true);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_slot,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_width,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_cd,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_wp,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_cmd,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_dat0,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_dat1,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_dat2,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_dat3,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_clk,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} }
    };

    // Parse the arguments.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_sdcard_obj_t *self = sd_card_obj_alloc();
    if (self == NULL) {
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("No available slots!"));
        return mp_const_none;
    }

    cy_rslt_t result = sd_card_init_helper(self, args);

    if (CY_RSLT_SUCCESS == result) {
        cyhal_sdhc_get_block_count(&self->sdhc_obj, (uint32_t *)&self->block_count);
    } else {
        if (cyhal_sdhc_is_card_inserted(&self->sdhc_obj) == false) {
            machine_sdcard_deinit(self);
            mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("SD Card not inserted!\n"));
        } else {
            machine_sdcard_deinit(self);
            mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("SD card init failed!\n"));
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_sdcard_readblocks(size_t n_args, const mp_obj_t *args) {

    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_address = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    if (n_args == 4) {
        block_address += mp_obj_get_int(args[3]);
    }

    size_t length = bufinfo.len / SDHC_BLOCK_SIZE;
    cy_rslt_t result = cyhal_sdhc_read(&self->sdhc_obj, block_address, bufinfo.buf, &length);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("machine_sdcard_readblocks() - SD Card Read failed !"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_sdcard_readblocks_obj, 3, 4, machine_sdcard_readblocks);

static mp_obj_t machine_sdcard_writeblocks(size_t n_args, const mp_obj_t *args) {

    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_address = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    size_t length = bufinfo.len / SDHC_BLOCK_SIZE;

    cy_rslt_t result = cyhal_sdhc_write(&self->sdhc_obj, block_address, bufinfo.buf, &length);
    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("machine_sdcard_writeblocks() - SD Card Write failed!"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_sdcard_writeblocks_obj, 3, 4, machine_sdcard_writeblocks);

static mp_obj_t machine_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    machine_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            machine_sdcard_deinit(self_in);
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->block_count);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SDHC_BLOCK_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in);
            cy_rslt_t result = cyhal_sdhc_erase(&self->sdhc_obj, offset, 1, 0);

            if (CY_RSLT_SUCCESS != result) {
                mp_raise_ValueError(MP_ERROR_TEXT("machine_sdcard_ioctl() - SD Card erase failed !"));
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_ioctl_obj, machine_sdcard_ioctl);


void mod_sdcard_deinit() {
    for (uint8_t i = 0; i < MAX_SDHC_SLOT; i++) {
        if (sdhc_obj[i] != NULL) {
            machine_sdcard_deinit(MP_OBJ_FROM_PTR(sdhc_obj[i]));
        }
    }
}

static const mp_rom_map_elem_t machine_sdcard_locals_dict_table[] = {
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_sdcard_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_sdcard_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(machine_sdcard_locals_dict, machine_sdcard_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_sdcard_type,
    MP_QSTR_SDCard,
    MP_TYPE_FLAG_NONE,
    make_new, machine_sdcard_make_new,
    locals_dict, &machine_sdcard_locals_dict
    );
