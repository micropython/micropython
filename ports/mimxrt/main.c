/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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
#include "py/stackctrl.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "ticks.h"
#include "tusb.h"
#include "led.h"
#include "pendsv.h"
#include "modmachine.h"

#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif
#include "systick.h"
#include "extmod/modnetwork.h"

extern uint8_t _sstack, _estack, _gc_heap_start, _gc_heap_end;

void board_init(void);

int main(void) {
    board_init();
    ticks_init();
    tusb_init();
    led_init();
    pendsv_init();

    mp_stack_set_top(&_estack);
    mp_stack_set_limit(&_estack - &_sstack - 1024);

    #if MICROPY_PY_LWIP
    // lwIP doesn't allow to reinitialise itself by subsequent calls to this function
    // because the system timeout list (next_timeout) is only ever reset by BSS clearing.
    // So for now we only init the lwIP stack once on power-up.
    lwip_init();
    #if LWIP_MDNS_RESPONDER
    mdns_resp_init();
    #endif
    systick_enable_dispatch(SYSTICK_DISPATCH_LWIP, mod_network_lwip_poll_wrapper);
    #endif

    for (;;) {
        gc_init(&_gc_heap_start, &_gc_heap_end);
        mp_init();

        #if MICROPY_PY_NETWORK
        mod_network_init();
        #endif

        // Initialise sub-systems.
        readline_init0();

        // Execute _boot.py to set up the filesystem.
        pyexec_frozen_module("_boot.py");

        // Execute user scripts.
        int ret = pyexec_file_if_exists("boot.py");
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
        machine_pin_irq_deinit();
        #if MICROPY_PY_MACHINE_I2S
        machine_i2s_deinit_all();
        #endif
        #if MICROPY_PY_NETWORK
        mod_network_deinit();
        #endif
        machine_pwm_deinit_all();
        gc_sweep_all();
        mp_deinit();
    }

    return 0;
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

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    mp_printf(MP_PYTHON_PRINTER, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    for (;;) {
    }
}
#endif

const char mimxrt_help_text[] =
    "Welcome to MicroPython!\n"
    "\n"
    "For online help please visit https://micropython.org/help/.\n"
    "\n"
    "For access to the hardware use the 'machine' module. \n"
    "\n"
    "Quick overview of some objects:\n"
    "  machine.Pin(pin) -- get a pin, eg machine.Pin(0)\n"
    "  machine.Pin(pin, m, [p]) -- get a pin and configure it for IO mode m, pull mode p\n"
    "    methods: init(..), value([v]), high(), low())\n"
    "\n"
    "    Pins are numbered board specific, either 0-n, or 'D0'-'Dn', or 'A0' - 'An',\n"
    "    according to the boards's pinout sheet.\n"
    "    Pin IO modes are: Pin.IN, Pin.OUT, Pin.OPEN_DRAIN\n"
    "    Pin pull modes are: Pin.PULL_UP, Pin.PULL_UP_47K, Pin.PULL_UP_22K, Pin.PULL_DOWN, Pin.PULL_HOLD\n"
    "  machine.ADC(pin) -- make an analog object from a pin\n"
    "    methods: read_u16()\n"
    "  machine.UART(id, baudrate=115200) -- create an UART object (id=1 - 8, board-specific)\n"
    "    methods: init(), write(buf), any()\n"
    "             buf=read(n), readinto(buf), buf=readline()\n"
    "    The RX and TX pins are fixed and board-specific.\n"
    "  machine.SoftI2C() -- create a Soft I2C object\n"
    "  machine.I2C(id) -- create a HW I2C object\n"
    "    methods: readfrom(addr, buf, stop=True), writeto(addr, buf, stop=True)\n"
    "             readfrom_mem(addr, memaddr, arg), writeto_mem(addr, memaddr, arg)\n"
    "    SoftI2C allows to use any pin for sda and scl, HW I2C id's and pins are fixed\n"
    "  machine.SoftSPI(baudrate=1000000) -- create a Soft SPI object\n"
    "  machine.SPI(id, baudrate=1000000) -- create a HW SPI object\n"
    "    methods: read(nbytes, write=0x00), write(buf), write_readinto(wr_buf, rd_buf)\n"
    "    SoftSPI allows to use any pin for SPI, HW SPI id's and pins are fixed\n"
    "  machine.Timer(id, freq, callback) -- create a hardware timer object (id=0,1,2)\n"
    "    eg: machine.Timer(freq=1, callback=lambda t:print(t))\n"
    "  machine.RTC() -- create a Real Time Clock object\n"
    "    methods: init(), datetime([dateime_tuple]), now()\n"
    "  machine.PWM(pin, freq, duty_u16[, kw_opts]) -- create a PWM object\n"
    "    methods: init(), duty_u16([value]), duty_ns([value]), freq([value])\n"
    "\n"
    "Useful control commands:\n"
    "  CTRL-C -- interrupt a running program\n"
    "  CTRL-D -- on a blank line, do a soft reset of the board\n"
    "  CTRL-E -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
    "For a list of available modules, type help('modules')\n"
;
