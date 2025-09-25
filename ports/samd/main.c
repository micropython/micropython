/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/softtimer.h"
#include "shared/tinyusb/mp_usbd.h"
#include "clock_config.h"

extern uint8_t _sstack, _estack, _sheap, _eheap;
extern void adc_deinit_all(void);
extern void pin_irq_deinit_all(void);
extern void pwm_deinit_all(void);
extern void sercom_deinit_all(void);

void samd_main(void) {
    mp_cstack_init_with_top(&_estack, &_estack - &_sstack);
    mp_hal_time_ns_set_from_rtc();

    for (;;) {
        gc_init(&_sheap, &_eheap);
        mp_init();

        // Initialise sub-systems.
        readline_init0();

        // Execute _boot.py to set up the filesystem.
        pyexec_frozen_module("_boot.py", false);

        // Execute user scripts.
        int ret = pyexec_file_if_exists("boot.py");

        mp_usbd_init();
        check_usb_clock_recovery_mode();

        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        // Do not execute main.py if boot.py failed
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
            ret = pyexec_file_if_exists("main.py");
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
        }

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
        mp_printf(MP_PYTHON_PRINTER, "MPY: soft reboot\n");
        #if MICROPY_PY_MACHINE_ADC
        adc_deinit_all();
        #endif
        pin_irq_deinit_all();
        #if MICROPY_PY_MACHINE_PWM
        pwm_deinit_all();
        #endif
        soft_timer_deinit();
        #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
        mp_usbd_deinit();
        #endif
        gc_sweep_all();
        #if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_UART
        sercom_deinit_all();
        #endif
        mp_deinit();
    }
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

void abort(void) {
    for (;;) {
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    mp_printf(MP_PYTHON_PRINTER, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    for (;;) {
    }
}
#endif
