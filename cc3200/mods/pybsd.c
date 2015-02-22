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
#include "simplelink.h"
#include "debug.h"
#include "mpexception.h"


#if MICROPY_HW_HAS_SDCARD

#define PYBSD_FREQUENCY_HZ      15000000        // 15MHz

static const pin_obj_t   *pybsd_pin_sd_detect;
static bool               pybsd_is_enabled;
static bool               pybsd_in_path;
static FATFS             *sd_fatfs;


void pybsd_init0 (void) {
    // Allocate memory for the sd file system
    ASSERT ((sd_fatfs = mem_Malloc(sizeof(FATFS))) != NULL);
}

bool pybsd_is_present(void) {
    if (pybsd_pin_sd_detect) {
        return pybsd_is_enabled && MAP_GPIOPinRead(pybsd_pin_sd_detect->port, pybsd_pin_sd_detect->bit);
    }
    return pybsd_is_enabled;
}

/******************************************************************************/
// Micro Python bindings
//

/// \method sd_config([value])
/// Configure the pins used for the sd card.
///   May receive 6 or 7 arguments. The 7th optional argument is the card detect pin.
///   this pin needs no external pull-up and must be low when the sdcard is inserted.
///
/// Usage:
///
///    pyb.SDcard.config_pins (d0_pin, d0_af, clk_pin, clk_af, cmd_pin, cmd_af)
///
///    or:
///
///    pyb.SDcard.sd_config_pins (d0_pin, d0_af, clk_pin, clk_af, cmd_pin, cmd_af, card_detect_pin)
///
STATIC mp_obj_t pybsd_config_pins (mp_uint_t n_args, const mp_obj_t *args) {
    byte pybsd_pin_d0, pybsd_pin_clk, pybsd_pin_cmd;
    byte pybsd_af_d0,  pybsd_af_clk,  pybsd_af_cmd;
    const pin_obj_t *pin;

    // get all the arguments
    pin = pin_find(args[1]);
    pybsd_pin_d0 = pin->pin_num;
    pybsd_af_d0 = mp_obj_get_int(args[2]);
    pin_verify_af (pybsd_af_d0);
    pin = pin_find(args[3]);
    pybsd_pin_clk = pin->pin_num;
    pybsd_af_clk = mp_obj_get_int(args[4]);
    pin_verify_af (pybsd_af_clk);
    pin = pin_find(args[5]);
    pybsd_pin_cmd = pin->pin_num;
    pybsd_af_cmd = mp_obj_get_int(args[6]);
    pin_verify_af (pybsd_af_cmd);

    // configure the sdhost pins
    MAP_PinTypeSDHost(pybsd_pin_d0, pybsd_af_d0);
    MAP_PinTypeSDHost(pybsd_pin_clk, pybsd_af_clk);
    MAP_PinTypeSDHost(pybsd_pin_cmd, pybsd_af_cmd);
    MAP_PinDirModeSet(pybsd_pin_clk, PIN_DIR_MODE_OUT);
    // configure the card detect pin if provided
    if (n_args == 8) {
        pybsd_pin_sd_detect = pin_find(args[7]);
        pin_config(pybsd_pin_sd_detect, PIN_MODE_0, GPIO_DIR_MODE_IN, PIN_TYPE_STD_PU, PIN_STRENGTH_4MA);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pybsd_config_pins_obj, 7, 8, pybsd_config_pins);

STATIC mp_obj_t pybsd_enable(mp_obj_t self) {
    // Enable SD peripheral clock
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // Reset MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    // Initialize MMCHS
    MAP_SDHostInit(SDHOST_BASE);
    // Configure the card clock
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), PYBSD_FREQUENCY_HZ);

    pybsd_is_enabled = true;

    // try to mount the sd card on /SD
    if (FR_OK != f_mount(sd_fatfs, "/SD", 1)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SD));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SD_slash_LIB));
    pybsd_in_path = true;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pybsd_enable_obj, pybsd_enable);

STATIC mp_obj_t pybsd_disable(mp_obj_t self) {
    if (pybsd_is_enabled) {
        pybsd_is_enabled = false;
        // unmount the sd card
        f_mount (NULL, "/SD", 1);
        // remove sd paths from mp_sys_path
        if (pybsd_in_path) {
            mp_obj_list_remove(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SD));
            mp_obj_list_remove(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SD_slash_LIB));
            pybsd_in_path = false;
        }
        // disable the peripheral
        MAP_PRCMPeripheralClkDisable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pybsd_disable_obj, pybsd_disable);

STATIC const mp_map_elem_t pybsd_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_config_pins),     (mp_obj_t)&pybsd_config_pins_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),          (mp_obj_t)&pybsd_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),         (mp_obj_t)&pybsd_disable_obj },
};
STATIC MP_DEFINE_CONST_DICT(pybsd_locals_dict, pybsd_locals_dict_table);

static const mp_obj_type_t sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SD,
    .locals_dict = (mp_obj_t)&pybsd_locals_dict,
};

const mp_obj_base_t pyb_sdcard_obj = {&sdcard_type};

#endif // MICROPY_HW_HAS_SDCARD
