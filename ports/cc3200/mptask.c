/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/mpconfig.h"
#include "py/stackctrl.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "shared/readline/readline.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "interrupt.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/gchelper.h"
#include "gccollect.h"
#include "mperror.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modusocket.h"
#include "modwlan.h"
#include "serverstask.h"
#include "telnet.h"
#include "debug.h"
#include "sflash_diskio.h"
#include "random.h"
#include "pybi2c.h"
#include "pins.h"
#include "mods/pybflash.h"
#include "pybsleep.h"
#include "pybtimer.h"
#include "cryptohash.h"
#include "mpirq.h"
#include "updater.h"
#include "moduos.h"
#include "antenna.h"
#include "task.h"

/******************************************************************************
 DECLARE PRIVATE CONSTANTS
 ******************************************************************************/

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void mptask_pre_init(void);
STATIC void mptask_init_sflash_filesystem(void);
STATIC void mptask_enter_ap_mode(void);
STATIC void mptask_create_main_py(void);

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
#ifdef DEBUG
OsiTaskHandle svTaskHandle;
#endif

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static fs_user_mount_t *sflash_vfs_fat;

static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";
static const char fresh_boot_py[] = "# boot.py -- run on boot-up\r\n"
    "# can run arbitrary Python, but best to keep it minimal\r\n"
                                    #if MICROPY_STDIO_UART
    "import os, machine\r\n"
    "os.dupterm(machine.UART(0, " MP_STRINGIFY(MICROPY_STDIO_UART_BAUD) "))\r\n"
                                    #endif
;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/

uintptr_t cortex_m3_get_sp(void);

void TASK_MicroPython(void *pvParameters) {
    // get the top of the stack to initialize the garbage collector
    uint32_t sp = cortex_m3_get_sp();

    bool safeboot = false;
    mptask_pre_init();

    #ifndef DEBUG
    safeboot = PRCMGetSpecialBit(PRCM_SAFE_BOOT_BIT);
    #endif

soft_reset:

    // Thread init
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif

    // initialise the stack pointer for the main thread (must be done after mp_thread_init)
    mp_stack_set_top((void *)sp);

    // GC init
    gc_init(&_boot, &_eheap);

    // MicroPython init
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_init(mp_sys_argv, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)

    // execute all basic initializations
    mp_irq_init0();
    pyb_sleep_init0();
    pin_init0();
    mperror_init0();
    uart_init0();
    timer_init0();
    readline_init0();
    mod_network_init0();
    rng_init0();

    pybsleep_reset_cause_t rstcause = pyb_sleep_get_reset_cause();
    if (rstcause < PYB_SLP_SOFT_RESET) {
        if (rstcause == PYB_SLP_HIB_RESET) {
            // when waking up from hibernate we just want
            // to enable simplelink and leave it as is
            wlan_first_start();
        } else {
            // only if not comming out of hibernate or a soft reset
            mptask_enter_ap_mode();
        }

        // enable telnet and ftp
        servers_start();
    }

    // initialize the serial flash file system
    mptask_init_sflash_filesystem();

    // append the flash paths to the system path
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));

    // reset config variables; they should be set by boot.py
    MP_STATE_PORT(machine_config_main) = MP_OBJ_NULL;

    if (!safeboot) {
        // run boot.py
        int ret = pyexec_file_if_exists("boot.py");
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        if (!ret) {
            // flash the system led
            mperror_signal_error();
        }
    }

    // now we initialise sub-systems that need configuration from boot.py,
    // or whose initialisation can be safely deferred until after running
    // boot.py.

    // at this point everything is fully configured and initialised.

    if (!safeboot) {
        // run the main script from the current directory.
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            const char *main_py;
            if (MP_STATE_PORT(machine_config_main) == MP_OBJ_NULL) {
                main_py = "main.py";
            } else {
                main_py = mp_obj_str_get_str(MP_STATE_PORT(machine_config_main));
            }
            int ret = pyexec_file_if_exists(main_py);
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
            if (!ret) {
                // flash the system led
                mperror_signal_error();
            }
        }
    }

    // main script is finished, so now go into REPL mode.
    // the REPL mode can change, or it can request a soft reset.
    for ( ; ;) {
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
    pyb_sleep_signal_soft_reset();
    mp_printf(&mp_plat_print, "MPY: soft reboot\n");

    // disable all callbacks to avoid undefined behaviour
    // when coming out of a soft reset
    mp_irq_disable_all();

    // cancel the RTC alarm which might be running independent of the irq state
    pyb_rtc_disable_alarm();

    // flush the serial flash buffer
    sflash_disk_flush();

    // clean-up the user socket space
    modusocket_close_all_user_sockets();

    // unmount all user file systems
    osmount_unmount_all();

    // wait for pending transactions to complete
    mp_hal_delay_ms(20);

    goto soft_reset;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
__attribute__ ((section(".boot")))
STATIC void mptask_pre_init(void) {
    // this one only makes sense after a poweron reset
    pyb_rtc_pre_init();

    // Create the simple link spawn task
    ASSERT(OSI_OK == VStartSimpleLinkSpawnTask(SIMPLELINK_SPAWN_TASK_PRIORITY));

    // Allocate memory for the flash file system
    ASSERT((sflash_vfs_fat = mem_Malloc(sizeof(*sflash_vfs_fat))) != NULL);

    // this one allocates memory for the nvic vault
    pyb_sleep_pre_init();

    // this one allocates memory for the WLAN semaphore
    wlan_pre_init();

    // this one allocates memory for the updater semaphore
    updater_pre_init();

    // this one allocates memory for the socket semaphore
    modusocket_pre_init();

    // CRYPTOHASH_Init();

    #ifndef DEBUG
    OsiTaskHandle svTaskHandle;
    #endif
    svTaskHandle = xTaskCreateStatic(TASK_Servers, "Servers",
        SERVERS_STACK_LEN, NULL, SERVERS_PRIORITY, svTaskStack, &svTaskTCB);
    ASSERT(svTaskHandle != NULL);
}

STATIC void mptask_init_sflash_filesystem(void) {
    FILINFO fno;

    // Initialise the local flash filesystem.
    // init the vfs object
    fs_user_mount_t *vfs_fat = sflash_vfs_fat;
    vfs_fat->blockdev.flags = 0;
    pyb_flash_init_vfs(vfs_fat);

    // Create it if needed, and mount in on /flash.
    FRESULT res = f_mount(&vfs_fat->fatfs);
    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, so create a fresh one
        uint8_t working_buf[FF_MAX_SS];
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT | FM_SFD, 0, working_buf, sizeof(working_buf));
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            __fatal_error("failed to create /flash");
        }
        // create empty main.py
        mptask_create_main_py();
    } else if (res == FR_OK) {
        // mount sucessful
        if (FR_OK != f_stat(&vfs_fat->fatfs, "/main.py", &fno)) {
            // create empty main.py
            mptask_create_main_py();
        }
    } else {
    fail:
        __fatal_error("failed to create /flash");
    }

    // mount the flash device (there should be no other devices mounted at this point)
    // we allocate this structure on the heap because vfs->next is a root pointer
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (vfs == NULL) {
        goto fail;
    }
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;

    // create /flash/sys, /flash/lib and /flash/cert if they don't exist
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/sys")) {
        f_mkdir(&vfs_fat->fatfs, "/sys");
    }
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/lib")) {
        f_mkdir(&vfs_fat->fatfs, "/lib");
    }
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/cert")) {
        f_mkdir(&vfs_fat->fatfs, "/cert");
    }

    f_chdir(&vfs_fat->fatfs, "/");

    // make sure we have a /flash/boot.py.  Create it if needed.
    res = f_stat(&vfs_fat->fatfs, "/boot.py", &fno);
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
        f_open(&vfs_fat->fatfs, &fp, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }
}

STATIC void mptask_enter_ap_mode(void) {
    // append the mac only if it's not the first boot
    bool add_mac = !PRCMGetSpecialBit(PRCM_FIRST_BOOT_BIT);
    // enable simplelink in ap mode (use the MAC address to make the ssid unique)
    wlan_sl_init(ROLE_AP, MICROPY_PORT_WLAN_AP_SSID, strlen(MICROPY_PORT_WLAN_AP_SSID),
        MICROPY_PORT_WLAN_AP_SECURITY, MICROPY_PORT_WLAN_AP_KEY, strlen(MICROPY_PORT_WLAN_AP_KEY),
        MICROPY_PORT_WLAN_AP_CHANNEL, ANTENNA_TYPE_INTERNAL, add_mac);
}

STATIC void mptask_create_main_py(void) {
    // create empty main.py
    FIL fp;
    f_open(&sflash_vfs_fat->fatfs, &fp, "/main.py", FA_WRITE | FA_CREATE_ALWAYS);
    UINT n;
    f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
    f_close(&fp);
}
