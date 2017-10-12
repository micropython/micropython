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
#include "py/lexer.h"
#include "py/parse.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/compile.h"
#include "lib/utils/pyexec.h"
#include "readline.h"
#include "gccollect.h"
#include "modmachine.h"
#include "modmusic.h"
#include "led.h"
#include "uart.h"
#include "nrf.h"
#include "pin.h"
#include "spi.h"
#include "i2c.h"
#include "rtc.h"
#if MICROPY_PY_MACHINE_HW_PWM
#include "pwm.h"
#endif
#include "timer.h"

#if (MICROPY_PY_BLE_NUS)
#include "ble_uart.h"
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
        mp_obj_t module_fun = mp_compile(&pn, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

extern uint32_t _heap_start;
extern uint32_t _heap_end;

int main(int argc, char **argv) {
    
soft_reset:
    mp_stack_set_top(&_ram_end);

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_set_limit((char*)&_ram_end - (char*)&_heap_end - 400);

    machine_init();

    gc_init(&_heap_start, &_heap_end);

    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_init(mp_sys_argv, 0);

    pyb_set_repl_info(MP_OBJ_NEW_SMALL_INT(0));

    readline_init0();

#if MICROPY_PY_MACHINE_HW_SPI
    spi_init0();
#endif

#if MICROPY_PY_MACHINE_I2C
    i2c_init0();
#endif

#if MICROPY_PY_MACHINE_HW_PWM
    pwm_init0();
#endif

#if MICROPY_PY_MACHINE_RTC
    rtc_init0();
#endif

#if MICROPY_PY_MACHINE_TIMER
    timer_init0();
#endif

    uart_init0();

#if (MICROPY_PY_BLE_NUS == 0)
    {
        mp_obj_t args[2] = {
            MP_OBJ_NEW_SMALL_INT(0),
            MP_OBJ_NEW_SMALL_INT(115200),
        };
        MP_STATE_PORT(pyb_stdio_uart) = machine_hard_uart_type.make_new((mp_obj_t)&machine_hard_uart_type, MP_ARRAY_SIZE(args), 0, args);
    }
#endif

pin_init0();

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
        vfs->flags = FSUSER_FREE_OBJ;
        sdcard_init_vfs(vfs);

        // put the sd device in slot 1 (it will be unused at this point)
        MP_STATE_PORT(fs_user_mount)[1] = vfs;

        FRESULT res = f_mount(&vfs->fatfs, vfs->str, 1);
        if (res != FR_OK) {
            printf("PYB: can't mount SD card\n");
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

#if (MICROPY_HW_HAS_LED)
    led_init();

    do_str("import pyb\r\n" \
           "pyb.LED(1).on()",
           MP_PARSE_FILE_INPUT);
#endif

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    int ret_code = 0;

#if MICROPY_PY_BLE_NUS
    ble_uart_init0();
    while (!ble_uart_enabled()) {
        ;
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

    mp_deinit();

    if (ret_code == PYEXEC_FORCED_EXIT) {
        NVIC_SystemReset();
    } else {
        goto soft_reset;
    }

    return 0;
}

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

STATIC mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    mp_raise_OSError(MP_EPERM);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif

void HardFault_Handler(void)
{
#if NRF52
	static volatile uint32_t reg;
	static volatile uint32_t reg2;
	static volatile uint32_t bfar;
	reg = SCB->HFSR;
	reg2 = SCB->CFSR;
	bfar = SCB->BFAR;
	for (int i = 0; i < 0; i++)
	{
		(void)reg;
		(void)reg2;
		(void)bfar;
	}
#endif
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
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

void _start(void) {main(0, NULL);}
