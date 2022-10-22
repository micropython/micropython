/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2020 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/softtimer.h"
#include "lib/oofatfs/ff.h"
#include "lib/littlefs/lfs1.h"
#include "lib/littlefs/lfs1_util.h"
#include "lib/littlefs/lfs2.h"
#include "lib/littlefs/lfs2_util.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "extmod/vfs_lfs.h"

#include "boardctrl.h"
#include "systick.h"
#include "pendsv.h"
#include "powerctrl.h"
#include "pybthread.h"
#include "gccollect.h"
#include "factoryreset.h"
#include "modmachine.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include "led.h"
#include "pin.h"
#include "extint.h"
#include "usrsw.h"
#include "rtc.h"
#include "storage.h"

#define RA_EARLY_PRINT  1       /* for enabling mp_print in boardctrl. */

#if MICROPY_PY_THREAD
STATIC pyb_thread_t pyb_thread_main;
#endif

#if defined(MICROPY_HW_UART_REPL)
#ifndef MICROPY_HW_UART_REPL_RXBUF
#define MICROPY_HW_UART_REPL_RXBUF (260)
#endif
STATIC machine_uart_obj_t machine_uart_repl_obj;
STATIC uint8_t machine_uart_repl_rxbuf[MICROPY_HW_UART_REPL_RXBUF];
#endif

void NORETURN __fatal_error(const char *msg) {
    for (volatile uint delay = 0; delay < 1000000; delay++) {
    }
    led_state(1, 1);
    led_state(2, 1);
    led_state(3, 1);
    led_state(4, 1);
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
    for (uint i = 0;;) {
        led_toggle(((i++) & 3) + 1);
        for (volatile uint delay = 0; delay < 1000000; delay++) {
        }
        if (i >= 16) {
            // to conserve power
            __WFI();
        }
    }
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    __fatal_error("");
}

void abort(void) {
    __fatal_error("abort");
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("");
}
#endif

STATIC mp_obj_t pyb_main(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_opt, MP_ARG_INT, {.u_int = 0} }
    };

    if (mp_obj_is_str(pos_args[0])) {
        MP_STATE_PORT(pyb_config_main) = pos_args[0];

        // parse args
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        #if MICROPY_ENABLE_COMPILER
        MP_STATE_VM(mp_optimise_value) = args[0].u_int;
        #endif
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_main_obj, 1, pyb_main);

#if MICROPY_HW_FLASH_MOUNT_AT_BOOT
// avoid inlining to avoid stack usage within main()
MP_NOINLINE STATIC bool init_flash_fs(uint reset_mode) {
    if (reset_mode == BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
        // Asked by user to reset filesystem
        factory_reset_create_filesystem();
    }

    // Default block device to entire flash storage
    mp_obj_t bdev = MP_OBJ_FROM_PTR(&pyb_flash_obj);

    int ret;

    #if MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2

    // Try to detect the block device used for the main filesystem based on the
    // contents of the superblock, which can be the first or second block.
    mp_int_t len = -1;
    uint8_t buf[64];
    for (size_t block_num = 0; block_num <= 1; ++block_num) {
        ret = storage_readblocks_ext(buf, block_num, 0, sizeof(buf));

        #if MICROPY_VFS_LFS1
        if (ret == 0 && memcmp(&buf[40], "littlefs", 8) == 0) {
            // LFS1
            lfs1_superblock_t *superblock = (void *)&buf[12];
            uint32_t block_size = lfs1_fromle32(superblock->d.block_size);
            uint32_t block_count = lfs1_fromle32(superblock->d.block_count);
            len = block_count * block_size;
            break;
        }
        #endif

        #if MICROPY_VFS_LFS2
        if (ret == 0 && memcmp(&buf[8], "littlefs", 8) == 0) {
            // LFS2
            lfs2_superblock_t *superblock = (void *)&buf[20];
            uint32_t block_size = lfs2_fromle32(superblock->block_size);
            uint32_t block_count = lfs2_fromle32(superblock->block_count);
            len = block_count * block_size;
            break;
        }
        #endif
    }

    if (len != -1) {
        // Detected a littlefs filesystem so create correct block device for it
        mp_obj_t args[] = { MP_OBJ_NEW_QSTR(MP_QSTR_len), MP_OBJ_NEW_SMALL_INT(len) };
        bdev = MP_OBJ_TYPE_GET_SLOT(&pyb_flash_type, make_new)(&pyb_flash_type, 0, 1, args);
    }

    #endif

    // Try to mount the flash on "/flash" and chdir to it for the boot-up directory.
    mp_obj_t mount_point = MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash);
    ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);

    if (ret == -MP_ENODEV && bdev == MP_OBJ_FROM_PTR(&pyb_flash_obj)
        && reset_mode != BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
        // No filesystem, bdev is still the default (so didn't detect a possibly corrupt littlefs),
        // and didn't already create a filesystem, so try to create a fresh one now.
        ret = factory_reset_create_filesystem();
        if (ret == 0) {
            ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);
        }
    }

    if (ret != 0) {
        printf("MPY: can't mount flash\n");
        return false;
    }

    return true;
}
#endif

void ra_main(uint32_t reset_mode) {
    // Hook for a board to run code at start up, for example check if a
    // bootloader should be entered instead of the main application.
    MICROPY_BOARD_STARTUP();

    // Initialize interrupt, systick and internal flash for RA.
    ra_init();

    MICROPY_BOARD_EARLY_INIT();

    // basic sub-system init
    #if MICROPY_PY_THREAD
    pyb_thread_init(&pyb_thread_main);
    #endif
    pendsv_init();
    led_init();
    #if MICROPY_HW_HAS_SWITCH
    switch_init0();
    #endif
    machine_init();
    #if MICROPY_HW_ENABLE_RTC
    rtc_init_start(false);
    #endif
    uart_init0();
    spi_init0();
    #if MICROPY_HW_ENABLE_STORAGE
    storage_init();
    #endif

    #if defined(MICROPY_HW_UART_REPL)
    // Set up a UART REPL using a statically allocated object
    machine_uart_repl_obj.base.type = &machine_uart_type;
    machine_uart_repl_obj.uart_id = MICROPY_HW_UART_REPL;
    machine_uart_repl_obj.is_static = true;
    machine_uart_repl_obj.timeout = 0;
    machine_uart_repl_obj.timeout_char = 2;
    uart_init(&machine_uart_repl_obj, MICROPY_HW_UART_REPL_BAUD, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, 0);
    uart_set_rxbuf(&machine_uart_repl_obj, sizeof(machine_uart_repl_rxbuf), machine_uart_repl_rxbuf);
    uart_attach_to_repl(&machine_uart_repl_obj, true);
    MP_STATE_PORT(machine_uart_obj_all)[MICROPY_HW_UART_REPL] = &machine_uart_repl_obj;
    #if RA_EARLY_PRINT
    MP_STATE_PORT(pyb_stdio_uart) = &machine_uart_repl_obj;
    #endif
    #endif

    boardctrl_state_t state;
    state.reset_mode = reset_mode;
    state.log_soft_reset = false;

    MICROPY_BOARD_BEFORE_SOFT_RESET_LOOP(&state);

soft_reset:

    MICROPY_BOARD_TOP_SOFT_RESET_LOOP(&state);

    // Python threading init
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    // Note: stack control relies on main thread being initialised above
    mp_stack_set_top(&_estack);
    mp_stack_set_limit((char *)&_estack - (char *)&_sstack - 1024);

    // GC init
    gc_init(MICROPY_HEAP_START, MICROPY_HEAP_END);

    #if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[384];
    mp_pystack_init(pystack, &pystack[384]);
    #endif

    // MicroPython init
    mp_init();

    // Initialise low-level sub-systems.  Here we need to very basic things like
    // zeroing out memory and resetting any of the sub-systems.  Following this
    // we can run Python scripts (eg boot.py), but anything that is configurable
    // by boot.py must be set after boot.py is run.

    #if defined(MICROPY_HW_UART_REPL)
    MP_STATE_PORT(pyb_stdio_uart) = &machine_uart_repl_obj;
    #else
    MP_STATE_PORT(pyb_stdio_uart) = NULL;
    #endif

    readline_init0();
    machine_pin_init();
    extint_init0();
    timer_init0();

    #if MICROPY_HW_ENABLE_I2S
    machine_i2s_init0();
    #endif

    // Initialise the local flash filesystem.
    // Create it if needed, mount in on /flash, and set it as current dir.
    bool mounted_flash = false;
    #if MICROPY_HW_FLASH_MOUNT_AT_BOOT
    mounted_flash = init_flash_fs(state.reset_mode);
    #endif

    // set sys.path based on mounted filesystems
    if (mounted_flash) {
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));
    }

    // reset config variables; they should be set by boot.py
    MP_STATE_PORT(pyb_config_main) = MP_OBJ_NULL;

    // Run optional frozen boot code.
    #ifdef MICROPY_BOARD_FROZEN_BOOT_FILE
    pyexec_frozen_module(MICROPY_BOARD_FROZEN_BOOT_FILE);
    #endif

    // Run boot.py (or whatever else a board configures at this stage).
    if (MICROPY_BOARD_RUN_BOOT_PY(&state) == BOARDCTRL_GOTO_SOFT_RESET_EXIT) {
        goto soft_reset_exit;
    }

    // Now we initialise sub-systems that need configuration from boot.py,
    // or whose initialisation can be safely deferred until after running
    // boot.py.

    // At this point everything is fully configured and initialised.

    // Run main.py (or whatever else a board configures at this stage).
    if (MICROPY_BOARD_RUN_MAIN_PY(&state) == BOARDCTRL_GOTO_SOFT_RESET_EXIT) {
        goto soft_reset_exit;
    }

    #if MICROPY_ENABLE_COMPILER
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
    #endif

soft_reset_exit:

    // soft reset

    MICROPY_BOARD_START_SOFT_RESET(&state);

    #if MICROPY_HW_ENABLE_STORAGE
    if (state.log_soft_reset) {
        mp_printf(&mp_plat_print, "MPY: sync filesystems\n");
    }
    storage_flush();
    #endif

    if (state.log_soft_reset) {
        mp_printf(&mp_plat_print, "MPY: soft reboot\n");
    }

    soft_timer_deinit();
    timer_deinit();
    uart_deinit_all();
    #if MICROPY_HW_ENABLE_DAC
    dac_deinit_all();
    #endif
    machine_pin_deinit();
    machine_deinit();

    #if MICROPY_PY_THREAD
    pyb_thread_deinit();
    #endif

    MICROPY_BOARD_END_SOFT_RESET(&state);

    gc_sweep_all();
    mp_deinit();

    goto soft_reset;
}

MP_REGISTER_ROOT_POINTER(mp_obj_t pyb_config_main);
