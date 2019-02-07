/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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
#define asm __asm

#include <stdio.h>
#include <string.h>

#include <sys/adi_core.h>
#include <sys/platform.h>
#if defined __ADSPSC57x__
#include <cdefSC57x_rom.h>
#endif
#if defined __ADSPSC58x__
#include <cdefSC58x_rom.h>
#endif
#include <services/pwr/adi_pwr.h>

#include "modmachine.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "lib/utils/pyexec.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "pin.h"
#include "sdcard.h"
#include "spi.h"

#define SC_RESET_SOFT      (0)
#define SC_RESET_SYSSRC    (1)
#define SC_RESET_HARD      (2)
#define SC_RESET_UNKNOWN   (3)

#if defined(__ADSP215xx__)
#if defined(CLOCK_SPEED_500MHZ) && defined(__ADSPSC589_FAMILY__)
#define NUM_FREQ  6u
static uint32_t validCore[NUM_FREQ] = {500u, 450u, 400u, 375u, 350u, 325u};
static uint32_t validSys [NUM_FREQ] = {250u, 225u, 200u, 125u, 175u,  65u};
#else
#define NUM_FREQ  5u
static uint32_t validCore[NUM_FREQ] = {450u, 400u, 375u, 350u, 325u};
static uint32_t validSys [NUM_FREQ] = {225u, 200u, 125u, 175u,  65u};
#endif
#else
#define NUM_FREQ  4u
static uint32_t validCore[NUM_FREQ] = {400u, 300u, 200u, 100u};
static uint32_t validSys [NUM_FREQ] = {200u, 200u, 100u,  50u};
#endif

/* the power service device number */
#define DEV_NUM   (0u)

/* default power settings */
#define DF_DEFAULT      (0x0u)
#define MSEL_DEFAULT    (0x10u)
#define SYSSEL_DEFAULT  (0x8u)
#define S0SEL_DEFAULT   (0x2u)
#define S1SEL_DEFAULT   (0x2u)
#define CSEL_DEFAULT    (0x4u)

#define MHZTOHZ       (1000000u)

#define CLKIN         (25u  * MHZTOHZ)

#define ADI_PWR_CLKOUT_CCLK   (ADI_PWR_CLKOUT_CLKO0)  /* CLKO0/4 */

STATIC uint32_t reset_cause;

STATIC uint32_t fcclk = 0u;
STATIC uint32_t fsysclk = 0u;
STATIC uint32_t fsclk0 = 0u;
STATIC uint32_t fsclk1 = 0u;

void check_result(ADI_PWR_RESULT result, char *message) {
    if (result != ADI_PWR_SUCCESS) {
        mp_raise_ValueError(message);
    }
}

uint32_t machine_get_fsclk0() {
    return fsclk0;
}

void machine_init(void) {
    ADI_PWR_RESULT result;
    uint32_t rcu_state = *pREG_RCU0_STAT;
    reset_cause = SC_RESET_UNKNOWN;
    if (rcu_state & (1 << 0))
        reset_cause = SC_RESET_HARD;
    else if (rcu_state & (1 << 2))
        reset_cause = SC_RESET_SYSSRC;
    else if (rcu_state & (1 << 3))
        reset_cause = SC_RESET_SOFT;
    result = adi_pwr_Init(DEV_NUM, CLKIN);
    check_result(result, "adi_pwr_Init");

    /* Set CGU clock select register */
    result = adi_pwr_SetClkOutSelectRegister(DEV_NUM, ADI_PWR_CLKOUT_CCLK);
    check_result(result, "adi_pwr_SetClkOutSelectRegister");

    result = adi_pwr_GetCoreFreq(DEV_NUM, &fcclk);
    check_result(result, "adi_pwr_GetCoreFreq");

    result = adi_pwr_GetSystemFreq(DEV_NUM, &fsysclk,  &fsclk0, &fsclk1);
    check_result(result, "adi_pwr_GetSystemFreq");
}

void machine_deinit(void) {
    // we are doing a soft-reset so change the reset_cause
    reset_cause = SC_RESET_SOFT;
}

// Print out lots of information about the board.
STATIC mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
    // print clock speed
    printf("CCLK: %lu MHz, SYSCLK: %lu MHz, SCLK0: %lu MHz, SCLK1: %lu MHz\n",
            fcclk / MHZTOHZ, fsysclk / MHZTOHZ, fsclk0 / MHZTOHZ, fsclk1 / MHZTOHZ);

    // to print info about memory
    /*{
        printf("_etext=%p\n", &_etext);
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }*/

    // qstr info
    {
        size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  %u total\n", info.total);
        printf("  %u : %u\n", info.used, info.free);
        printf("  1=%u 2=%u m=%u\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    /*{
        #if MICROPY_VFS_FAT
        for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
            if (strncmp("/flash", vfs->str, vfs->len) == 0) {
                // assumes that it's a FatFs filesystem
                fs_user_mount_t *vfs_fat = MP_OBJ_TO_PTR(vfs->obj);
                DWORD nclst;
                f_getfree(&vfs_fat->fatfs, &nclst);
                printf("LFS free: %u bytes\n", (uint)(nclst * vfs_fat->fatfs.csize * 512));
                break;
            }
        }
        #endif
    }*/

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

// Returns a string of 6 bytes, which is the MAC address of the board.
STATIC mp_obj_t machine_unique_id(void) {
    uint32_t gp1[16];
    uint32_t *addr = (uint32_t *)0x24000300;
    for (int i = 0; i < 16; i++)
        gp1[i] = *addr++;
    //adi_rom_otp_get(otpcmd_gp1, gp1);
    return mp_obj_new_bytes((const byte*)gp1, 6);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

// Resets the SC5xx processor
STATIC mp_obj_t machine_reset(void) {
    *pREG_RCU0_STAT |= (1 << 3);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

// get or set the MCU frequencies
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    ADI_PWR_RESULT result;

    if (n_args == 0) {
        // get
        result = adi_pwr_GetCoreFreq(DEV_NUM, &fcclk);
        check_result(result, "adi_pwr_GetCoreFreq");
        result = adi_pwr_GetSystemFreq(DEV_NUM, &fsysclk,  &fsclk0, &fsclk1);
        check_result(result, "adi_pwr_GetSystemFreq");
        mp_obj_t tuple[] = {
           mp_obj_new_int(fcclk),
           mp_obj_new_int(fsysclk),
           mp_obj_new_int(fsclk0),
           mp_obj_new_int(fsclk1)
        };
        return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
    } else {
        // set
        mp_int_t sysclk = mp_obj_get_int(args[0]);
        // find the match
        mp_int_t n;
        for (n = 0; n < NUM_FREQ; n++) {
            if (validCore[n] == sysclk)
                break;
        }
        if (validCore[n] != sysclk) {
            mp_raise_ValueError("Invalid frequency");
        }
        result = adi_pwr_SetFreq(DEV_NUM, validCore[n]*MHZTOHZ, validSys[n]*MHZTOHZ);
        check_result(result, "adi_pwr_SetFreq");
        result = adi_pwr_GetCoreFreq(DEV_NUM, &fcclk);
        check_result(result, "adi_pwr_GetCoreFreq");
        result = adi_pwr_GetSystemFreq(DEV_NUM, &fsysclk,  &fsclk0, &fsclk1);
        check_result(result, "adi_pwr_GetSystemFreq");
        if ((fcclk != validCore[n]*MHZTOHZ) || (fsysclk != validSys[n]*MHZTOHZ)) {
            mp_raise_ValueError("Unable to change frequency!");
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 4, machine_freq);

STATIC mp_obj_t machine_reset_cause(void) {
    return MP_OBJ_NEW_SMALL_INT(reset_cause);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset),          MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },

    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_Signal),              MP_ROM_PTR(&machine_signal_type) },

    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&machine_hard_spi_type) },

    { MP_ROM_QSTR(MP_QSTR_SD),                  MP_ROM_PTR(&sdcard_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDCard),              MP_ROM_PTR(&sdcard_type) },

    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(SC_RESET_SOFT) },
    { MP_ROM_QSTR(MP_QSTR_SYSSRC_RESET),        MP_ROM_INT(SC_RESET_SYSSRC) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(SC_RESET_HARD) },
    { MP_ROM_QSTR(MP_QSTR_UNKNOWN_RESET),       MP_ROM_INT(SC_RESET_UNKNOWN) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

