/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "sdhost.h"
#include "pybpin.h"
#include "pybsd.h"
#include "ff.h"
#include "diskio.h"
#include "sd_diskio.h"
#include "simplelink.h"
#include "debug.h"
#include "mpexception.h"
#include "pybsleep.h"


#if MICROPY_HW_HAS_SDCARD

#define PYBSD_FREQUENCY_HZ              15000000    // 15MHz

typedef struct {
    mp_obj_base_t base;
    FATFS        *fatfs;
    pin_obj_t    *pin_clk;
    bool         pinsset;
    bool         enabled;
} pybsd_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pybsd_obj_t pybsd_obj;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC mp_obj_t pybsd_make_new (mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
STATIC mp_obj_t pybsd_disable (mp_obj_t self_in);
STATIC mp_obj_t pybsd_enable (mp_obj_t self_in);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void pybsd_init0 (void) {
    // allocate memory for the sd file system
    ASSERT ((pybsd_obj.fatfs = mem_Malloc(sizeof(FATFS))) != NULL);
}

void pybsd_deinit (void) {
    pybsd_disable ((mp_obj_t)&pybsd_obj);
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
/// Initalizes the sd card driver
STATIC void pybsd_init (pybsd_obj_t *self) {
    // Configure the clock pin as output only
    MAP_PinDirModeSet(self->pin_clk->pin_num, PIN_DIR_MODE_OUT);
    // Enable SD peripheral clock
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // Reset MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    // Initialize MMCHS
    MAP_SDHostInit(SDHOST_BASE);
    // Configure the card clock
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), PYBSD_FREQUENCY_HZ);
}

/******************************************************************************/
// Micro Python bindings
//

/// \classmethod \constructor()
/// Configure the pins used for the sd card.
///   May receive 0, or 6 arguments.
///
/// Usage:
///    sd = pyb.SD()
////
///    sd = pyb.SD(d0_pin, d0_af, clk_pin, clk_af, cmd_pin, cmd_af)
///
STATIC mp_obj_t pybsd_make_new (mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 6, false);

    if (n_args == 6) {
        // save the clock pin for later use
        pybsd_obj.pin_clk = (pin_obj_t *)pin_find(args[2]);

        // configure the data pin with pull-up enabled
        pin_config ((pin_obj_t *)pin_find(args[0]), mp_obj_get_int(args[1]), 0, PIN_TYPE_STD_PU, PIN_STRENGTH_4MA);
        // configure the clock pin
        pin_config (pybsd_obj.pin_clk, mp_obj_get_int(args[3]), 0, PIN_TYPE_STD, PIN_STRENGTH_4MA);
        // configure the command pin with pull-up enabled
        pin_config ((pin_obj_t *)pin_find(args[4]), mp_obj_get_int(args[5]), 0, PIN_TYPE_STD_PU, PIN_STRENGTH_4MA);

        pybsd_obj.pinsset = true;
        pybsd_obj.base.type = &pyb_sd_type;
    }
    else if (!pybsd_obj.pinsset) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
    }

    return &pybsd_obj;
}

/// \method enable()
/// Enables the sd card and mounts the file system
STATIC mp_obj_t pybsd_enable (mp_obj_t self_in) {
    pybsd_obj_t *self = self_in;

    if (!self->enabled) {
        // do the init first
        pybsd_init (self);

        // try to mount the sd card on /sd
        if (FR_OK != f_mount(self->fatfs, "/sd", 1)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }

        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));

        // register it with the sleep module
        pybsleep_add ((const mp_obj_t)&pybsd_obj, (WakeUpCB_t)pybsd_init);
        self->enabled = true;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pybsd_enable_obj, pybsd_enable);

/// \method disable()
/// Disables the sd card and unmounts the file system
STATIC mp_obj_t pybsd_disable (mp_obj_t self_in) {
    pybsd_obj_t *self = self_in;
    if (self->enabled) {
        self->enabled = false;
        // unmount the sd card
        f_mount (NULL, "/sd", 1);
        // remove sd paths from mp_sys_path
        mp_obj_list_remove(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
        mp_obj_list_remove(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));

        // disable the peripheral
        MAP_PRCMPeripheralClkDisable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

        // de-initialze de sd card at diskio level
        sd_disk_deinit();

        // unregister it with the sleep module
        pybsleep_remove (self);

        // change the drive in case it was /sd
        f_chdrive("/flash");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pybsd_disable_obj, pybsd_disable);

STATIC const mp_map_elem_t pybsd_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),          (mp_obj_t)&pybsd_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),         (mp_obj_t)&pybsd_disable_obj },
};
STATIC MP_DEFINE_CONST_DICT(pybsd_locals_dict, pybsd_locals_dict_table);

const mp_obj_type_t pyb_sd_type = {
    { &mp_type_type },
    .name = MP_QSTR_SD,
    .make_new = pybsd_make_new,
    .locals_dict = (mp_obj_t)&pybsd_locals_dict,
};

#endif // MICROPY_HW_HAS_SDCARD
