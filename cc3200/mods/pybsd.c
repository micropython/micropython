/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "sdhost.h"
#include "sd_diskio.h"
#include "pybsd.h"
#include "mpexception.h"
#include "pybsleep.h"
#include "pybpin.h"
#include "pins.h"

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define PYBSD_FREQUENCY_HZ              15000000    // 15MHz

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
pybsd_obj_t pybsd_obj;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_obj_t pyb_sd_def_pin[3] = {&pin_GP10, &pin_GP11, &pin_GP15};

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pyb_sd_hw_init (pybsd_obj_t *self);
STATIC mp_obj_t pyb_sd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_obj_t pyb_sd_deinit (mp_obj_t self_in);

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
/// Initalizes the sd card hardware driver
STATIC void pyb_sd_hw_init (pybsd_obj_t *self) {
    if (self->pin_clk) {
        // Configure the clock pin as output only
        MAP_PinDirModeSet(((pin_obj_t *)(self->pin_clk))->pin_num, PIN_DIR_MODE_OUT);
    }
    // Enable SD peripheral clock
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // Reset MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    // Initialize MMCHS
    MAP_SDHostInit(SDHOST_BASE);
    // Configure the card clock
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), PYBSD_FREQUENCY_HZ);
    // Set card rd/wr block len
    MAP_SDHostBlockSizeSet(SDHOST_BASE, SD_SECTOR_SIZE);
    self->enabled = true;
}

STATIC mp_obj_t pyb_sd_init_helper (pybsd_obj_t *self, const mp_arg_val_t *args) {
    // assign the pins
    mp_obj_t pins_o = args[0].u_obj;
    if (pins_o != mp_const_none) {
        mp_obj_t *pins;
        if (pins_o == MP_OBJ_NULL) {
            // use the default pins
            pins = (mp_obj_t *)pyb_sd_def_pin;
        } else {
            mp_obj_get_array_fixed_n(pins_o, MP_ARRAY_SIZE(pyb_sd_def_pin), &pins);
        }
        pin_assign_pins_af (pins, MP_ARRAY_SIZE(pyb_sd_def_pin), PIN_TYPE_STD_PU, PIN_FN_SD, 0);
        // save the pins clock
        self->pin_clk = pin_find(pins[0]);
    }

    pyb_sd_hw_init (self);
    if (sd_disk_init() != 0) {
        mp_raise_OSError(MP_EIO);
    }

    // register it with the sleep module
    pyb_sleep_add ((const mp_obj_t)self, (WakeUpCB_t)pyb_sd_hw_init);
    return mp_const_none;
}

/******************************************************************************/
// MicroPython bindings
//

STATIC const mp_arg_t pyb_sd_init_args[] = {
    { MP_QSTR_id,                          MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pins,                        MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
STATIC mp_obj_t pyb_sd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_sd_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_sd_init_args, args);

    // check the peripheral id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // setup and initialize the object
    mp_obj_t self = &pybsd_obj;
    pybsd_obj.base.type = &pyb_sd_type;
    pyb_sd_init_helper (self, &args[1]);
    return self;
}

STATIC mp_obj_t pyb_sd_init (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_sd_init_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &pyb_sd_init_args[1], args);
    return pyb_sd_init_helper(pos_args[0], args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_sd_init_obj, 1, pyb_sd_init);

STATIC mp_obj_t pyb_sd_deinit (mp_obj_t self_in) {
    pybsd_obj_t *self = self_in;
    // disable the peripheral
    self->enabled = false;
    MAP_PRCMPeripheralClkDisable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // de-initialze the sd card at diskio level
    sd_disk_deinit();
    // unregister it from the sleep module
    pyb_sleep_remove (self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sd_deinit_obj, pyb_sd_deinit);

STATIC mp_obj_t pyb_sd_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    DRESULT res = sd_disk_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SD_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_readblocks_obj, pyb_sd_readblocks);

STATIC mp_obj_t pyb_sd_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    DRESULT res = sd_disk_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SD_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_writeblocks_obj, pyb_sd_writeblocks);

STATIC mp_obj_t pyb_sd_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:
        case BP_IOCTL_DEINIT:
        case BP_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(sd_disk_info.ulNofBlock * (sd_disk_info.ulBlockSize / 512));

        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SD_SECTOR_SIZE);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sd_ioctl_obj, pyb_sd_ioctl);

STATIC const mp_rom_map_elem_t pyb_sd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_sd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_sd_deinit_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_sd_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_sd_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_sd_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_sd_locals_dict, pyb_sd_locals_dict_table);

const mp_obj_type_t pyb_sd_type = {
    { &mp_type_type },
    .name = MP_QSTR_SD,
    .make_new = pyb_sd_make_new,
    .locals_dict = (mp_obj_t)&pyb_sd_locals_dict,
};
