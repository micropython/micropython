/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Glenn Ruben Bakke
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
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/compile.h"
#include "extmod/modmachine.h"
#include "shared/runtime/pyexec.h"
#include "readline.h"
#include "gccollect.h"
#include "modmachine.h"
#include "modmusic.h"
#include "modules/os/microbitfs.h"
#include "led.h"
#include "uart.h"
#include "nrf.h"
#include "pin.h"
#include "spi.h"
#include "i2c.h"
#include "adc.h"
#include "rtcounter.h"

#if MICROPY_PY_MACHINE_HW_PWM
#include "pwm.h"
#endif
#include "timer.h"

#if BLUETOOTH_SD
#include "nrf_sdm.h"
#endif

#if (MICROPY_PY_BLE_NUS)
#include "ble_uart.h"
#endif

#if MICROPY_PY_MACHINE_SOFT_PWM
#include "ticker.h"
#include "softpwm.h"
#endif

#if MICROPY_HW_ENABLE_USBDEV && MICROPY_HW_USB_CDC
#include "usb_cdc.h"
#endif

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "flashbdev.h"
#endif

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t pn = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&pn, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

extern uint32_t _heap_start;
extern uint32_t _heap_end;

void NORETURN _start(void) {
    // Hook for a board to run code at start up, for example check if a
    // bootloader should be entered instead of the main application.
    MICROPY_BOARD_STARTUP();

    MICROPY_BOARD_EARLY_INIT();

soft_reset:

    #if MICROPY_PY_TIME_TICKS
    rtc1_init_time_ticks();
    #endif

    led_init();

    led_state(1, 1); // MICROPY_HW_LED_1 aka MICROPY_HW_LED_RED

    mp_stack_set_top(&_ram_end);

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_set_limit((char *)&_ram_end - (char *)&_heap_end - 400);

    machine_init();

    gc_init(&_heap_start, &_heap_end);

    mp_init();
    readline_init0();

    #if MICROPY_PY_MACHINE_SPI
    spi_init0();
    #endif

    #if MICROPY_PY_MACHINE_I2C
    i2c_init0();
    #endif

    #if MICROPY_PY_MACHINE_ADC
    adc_init0();
    #endif

    #if MICROPY_PY_MACHINE_HW_PWM
    pwm_init0();
    #endif

    #if MICROPY_PY_MACHINE_RTCOUNTER
    rtc_init0();
    #endif

    #if MICROPY_PY_MACHINE_TIMER_NRF
    timer_init0();
    #endif

    #if MICROPY_PY_MACHINE_UART
    uart_init0();
    #endif

    #if MICROPY_HW_ENABLE_UART_REPL
    {
        mp_obj_t args[2] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_UART_REPL),
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_UART_REPL_BAUD),
        };
        MP_STATE_VM(dupterm_objs[0]) = MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, make_new)((mp_obj_t)&machine_uart_type, MP_ARRAY_SIZE(args), 0, args);
    }
    #endif

    pin_init0();

    #if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
    flashbdev_init();

    // Try to mount the flash on "/flash" and chdir to it for the boot-up directory.
    mp_obj_t mount_point = MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash);
    int ret = mp_vfs_mount_and_chdir_protected((mp_obj_t)&nrf_flash_obj, mount_point);

    if ((ret == -MP_ENODEV) || (ret == -MP_EIO)) {
        pyexec_frozen_module("_mkfs.py", false); // Frozen script for formatting flash filesystem.
        ret = mp_vfs_mount_and_chdir_protected((mp_obj_t)&nrf_flash_obj, mount_point);
    }

    if (ret != 0) {
        printf("MPY: can't mount flash\n");
    } else {
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));
    }
    #endif

    #if MICROPY_MBFS
    microbit_filesystem_init();
    #endif

    #if MICROPY_HW_HAS_SDCARD
    // if an SD card is present then mount it on /sd/
    if (sdcard_is_present()) {
        // create vfs object
        fs_user_mount_t *vfs = m_new_obj_maybe(fs_user_mount_t);
        if (vfs == NULL) {
            goto no_mem_for_sd;
        }
        vfs->str = "/sd";
        vfs->len = 3;
        vfs->flags = MP_BLOCKDEV_FLAG_FREE_OBJ;
        sdcard_init_vfs(vfs);

        // put the sd device in slot 1 (it will be unused at this point)
        MP_STATE_PORT(fs_user_mount)[1] = vfs;

        FRESULT res = f_mount(&vfs->fatfs, vfs->str, 1);
        if (res != FR_OK) {
            printf("MPY: can't mount SD card\n");
            MP_STATE_PORT(fs_user_mount)[1] = NULL;
            m_del_obj(fs_user_mount_t, vfs);
        } else {
            // TODO these should go before the /flash entries in the path
            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));

            // use SD card as current directory
            f_chdrive("/sd");
        }
    no_mem_for_sd:;
    }
    #endif

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    int ret_code = 0;

    #if MICROPY_PY_BLE_NUS
    ble_uart_init0();
    #endif

    #if MICROPY_PY_MACHINE_SOFT_PWM
    ticker_init0();
    softpwm_init0();
    #endif

    #if MICROPY_PY_MUSIC
    microbit_music_init0();
    #endif
    #if BOARD_SPECIFIC_MODULES
    board_modules_init0();
    #endif

    #if MICROPY_PY_MACHINE_SOFT_PWM
    ticker_start();
    pwm_start();
    #endif

    led_state(1, 0);

    #if MICROPY_VFS || MICROPY_MBFS || MICROPY_MODULE_FROZEN
    ret_code = pyexec_file_if_exists("boot.py");
    #endif

    #if MICROPY_HW_ENABLE_USBDEV && MICROPY_HW_USB_CDC
    usb_cdc_init();
    #endif

    #if MICROPY_VFS || MICROPY_MBFS || MICROPY_MODULE_FROZEN
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret_code != 0) {
        pyexec_file_if_exists("main.py");
    }
    #endif

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            ret_code = pyexec_friendly_repl();
            if (ret_code != 0) {
                break;
            }
        }
    }

    #if MICROPY_PY_MACHINE_HW_PWM
    pwm_deinit_all();
    #endif

    mp_deinit();

    printf("MPY: soft reboot\n");

    #if BLUETOOTH_SD
    sd_softdevice_disable();
    #endif

    goto soft_reset;
}

#if !MICROPY_VFS
#if MICROPY_MBFS
// Use micro:bit filesystem
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    return os_mbfs_new_reader(qstr_str(filename));
}

mp_import_stat_t mp_import_stat(const char *path) {
    return os_mbfs_import_stat(path);
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return os_mbfs_open(n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#else
// use dummy functions - no filesystem available
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    mp_raise_OSError(MP_EPERM);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif
#endif


void HardFault_Handler(void) {
    #if defined(NRF52_SERIES) || defined(NRF91_SERIES)
    static volatile uint32_t reg;
    static volatile uint32_t reg2;
    static volatile uint32_t bfar;
    reg = SCB->HFSR;
    reg2 = SCB->CFSR;
    bfar = SCB->BFAR;
    for (int i = 0; i < 0; i++) {
        (void)reg;
        (void)reg2;
        (void)bfar;
    }
    #endif
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)val);
    __fatal_error("");
}

void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
