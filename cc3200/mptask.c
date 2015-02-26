/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdint.h>
#include <std.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "pin.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "pyexec.h"
#include "gccollect.h"
#include "gchelper.h"
#include "readline.h"
#include "mptask.h"
#include "mperror.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "serverstask.h"
#include "telnet.h"
#include "debug.h"
#include "ff.h"
#include "diskio.h"
#include "sflash_diskio.h"
#include "mpexception.h"
#include "random.h"
#include "pybextint.h"
#include "pybi2c.h"
#include "pybsd.h"
#include "pins.h"

/******************************************************************************
 DECLARE PRIVATE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void main_init_sflash_filesystem (void);
STATIC void main_enter_ap_mode (void);
STATIC void main_create_main_py (void);

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
#ifdef DEBUG
OsiTaskHandle   svTaskHandle;
#endif

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static FATFS *sflash_fatfs;

static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";
static const char fresh_boot_py[] = "# boot.py -- run on boot-up\r\n"
                                    "# can run arbitrary Python, but best to keep it minimal\r\n";

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/

void TASK_Micropython (void *pvParameters) {
    // Initialize the garbage collector with the top of our stack
    uint32_t sp = gc_helper_get_sp();
    gc_collect_init (sp);
    bool safeboot;
    FRESULT res;

#if MICROPY_HW_ENABLE_RTC
    pybrtc_init();
#endif

#ifdef DEBUG
    safeboot = false;
#else
    safeboot = mperror_safe_boot_requested();
#endif

    // Create the simple link spawn task
    ASSERT (OSI_OK == VStartSimpleLinkSpawnTask(SIMPLELINK_SPAWN_TASK_PRIORITY));

    // Allocate memory for the flash file system
    ASSERT ((sflash_fatfs = mem_Malloc(sizeof(FATFS))) != NULL);
#if MICROPY_HW_HAS_SDCARD
    pybsd_init0();
#endif

#ifdef DEBUG
    ASSERT (OSI_OK == osi_TaskCreate(TASK_Servers,
                                     (const signed char *)"Servers",
                                     SERVERS_STACK_SIZE, NULL, SERVERS_PRIORITY, &svTaskHandle));
#else
    ASSERT (OSI_OK == osi_TaskCreate(TASK_Servers,
                                     (const signed char *)"Servers",
                                     SERVERS_STACK_SIZE, NULL, SERVERS_PRIORITY, NULL));
#endif

soft_reset:

    // GC init
    gc_init(&_heap, &_eheap);

    // Micro Python init
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_init(mp_sys_argv, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)

    mperror_init0();
    mpexception_init0();
    uart_init0();
    pin_init0();

    // configure stdio uart pins with the correct af
    // param 3 ("mode") is DON'T CARE" for AFs others than GPIO
    pin_config(&pin_GPIO1, PIN_MODE_3, 0, PIN_TYPE_STD, PIN_STRENGTH_2MA);
    pin_config(&pin_GPIO2, PIN_MODE_3, 0, PIN_TYPE_STD, PIN_STRENGTH_2MA);
    // Instantiate the stdio uart
    mp_obj_t args[2] = {
            mp_obj_new_int(MICROPY_STDIO_UART),
            mp_obj_new_int(MICROPY_STDIO_UART_BAUD),
    };
    pyb_stdio_uart = pyb_uart_type.make_new((mp_obj_t)&pyb_uart_type, MP_ARRAY_SIZE(args), 0, args);

    readline_init0();
    extint_init0();
    mod_network_init();
    wlan_init0();
#if MICROPY_HW_ENABLE_RNG
    rng_init0();
#endif

    mperror_enable_heartbeat();

    main_enter_ap_mode();

    // enable telnet and ftp servers
    servers_enable();

    // initialize the serial flash file system
    main_init_sflash_filesystem();

    // append the SFLASH paths to the system path
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SFLASH));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_SFLASH_slash_LIB));

    // reset config variables; they should be set by boot.py
    MP_STATE_PORT(pyb_config_main) = MP_OBJ_NULL;

    if (!safeboot) {
        // run boot.py, if it exists
        const char *boot_py = "BOOT.PY";
        res = f_stat(boot_py, NULL);
        if (res == FR_OK) {
            int ret = pyexec_file(boot_py);
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
            if (!ret) {
                // flash the system led
                mperror_signal_error();
            }
        }
    }

    // Now we initialise sub-systems that need configuration from boot.py,
    // or whose initialisation can be safely deferred until after running
    // boot.py.

    // At this point everything is fully configured and initialised.

    if (!safeboot) {
        // Run the main script from the current directory.
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            const char *main_py;
            if (MP_STATE_PORT(pyb_config_main) == MP_OBJ_NULL) {
                main_py = "MAIN.PY";
            } else {
                main_py = mp_obj_str_get_str(MP_STATE_PORT(pyb_config_main));
            }
            res = f_stat(main_py, NULL);
            if (res == FR_OK) {
                int ret = pyexec_file(main_py);
                if (ret & PYEXEC_FORCED_EXIT) {
                    goto soft_reset_exit;
                }
                if (!ret) {
                    // flash the system led
                    mperror_signal_error();
                }
            }
        }
    }

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

soft_reset_exit:

    // soft reset

    sflash_disk_flush();

#if MICROPY_HW_HAS_SDCARD
    pybsd_deinit();
#endif

    printf("PYB: soft reboot\n");

    // wait for all bus transfers to complete
    HAL_Delay(50);

    // disable wlan services
    wlan_stop_servers();
    wlan_stop();

    uart_deinit();

    goto soft_reset;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

STATIC void main_init_sflash_filesystem (void) {
    // Initialise the local flash filesystem.
    // Create it if needed, and mount in on /sflash.
    // try to mount the flash
    FRESULT res = f_mount(sflash_fatfs, "/SFLASH", 1);
    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, so create a fresh one
        res = f_mkfs("/SFLASH", 1, 0);
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            __fatal_error("could not create /SFLASH file system");
        }
        // create empty main.py
        main_create_main_py();
    } else if (res == FR_OK) {
        // mount sucessful
        FILINFO fno;
        if (FR_OK != f_stat("/SFLASH/MAIN.PY", &fno)) {
            // create empty main.py
            main_create_main_py();
        }
    } else {
        __fatal_error("could not create /SFLASH file system");
    }

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    f_chdrive("/SFLASH");

    // Make sure we have a /flash/boot.py.  Create it if needed.
    FILINFO fno;
    res = f_stat("/SFLASH/BOOT.PY", &fno);
    if (res == FR_OK) {
        if (fno.fattrib & AM_DIR) {
            // exists as a directory
            // TODO handle this case
            // see http://elm-chan.org/fsw/ff/img/app2.c for a "rm -rf" implementation
        } else {
            // exists as a file, good!
        }
    } else {
        // doesn't exist, create fresh file
        FIL fp;
        f_open(&fp, "/SFLASH/BOOT.PY", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }
}

STATIC void main_enter_ap_mode (void) {
    // Enable simplelink in low power mode
    wlan_sl_enable (ROLE_AP, SERVERS_DEF_AP_SSID, strlen(SERVERS_DEF_AP_SSID), SERVERS_DEF_AP_SECURITY,
                    SERVERS_DEF_AP_KEY, strlen(SERVERS_DEF_AP_KEY), SERVERS_DEF_AP_CHANNEL);
    wlan_set_pm_policy (SL_NORMAL_POLICY);
}

STATIC void main_create_main_py (void) {
    // create empty main.py
    FIL fp;
    f_open(&fp, "/SFLASH/MAIN.PY", FA_WRITE | FA_CREATE_ALWAYS);
    UINT n;
    f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
    f_close(&fp);
}

STATIC mp_obj_t pyb_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        MP_STATE_PORT(pyb_config_main) = main;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_main_obj, pyb_main);
