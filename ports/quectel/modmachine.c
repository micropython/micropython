/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include <stdint.h>
#include "mpconfigport.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/mperrno.h"

#include "modmachine.h"
#include "shared/runtime/pyexec.h"
#include "mphalport.h"

#if MICROPY_QPY_MODULE_MACHINE
#if MICROPY_PY_SOFT_RESET
static mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    MP_STATE_VM(mp_softreset_exception).traceback_data = NULL;
    MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_softreset_exception));
#if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
#endif

    mp_mthread_wakeup();

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);
#endif


static const mp_rom_map_elem_t machine_module_globals_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine) },
#if MICROPY_QPY_MACHINE_PIN
	{ MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },
#endif
#if MICROPY_QPY_MACHINE_EXTINT
	{ MP_ROM_QSTR(MP_QSTR_ExtInt), MP_ROM_PTR(&machine_extint_type) },
#endif
#if MICROPY_QPY_MACHINE_UART
	{ MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
#endif
#if MICROPY_QPY_MACHINE_SPI
	{ MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_hard_spi_type) },
#endif
#if MICROPY_QPY_MACHINE_I2C
	{ MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_hard_i2c_type) },
#endif
#if MICROPY_QPY_MACHINE_I2C_SOFT
	{ MP_ROM_QSTR(MP_QSTR_I2C_simulation), MP_ROM_PTR(&machine_simulation_i2c_type) },
#endif
#if MICROPY_QPY_MACHINE_TIMER
    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) },
#endif
#if MICROPY_PY_SOFT_RESET
	{ MP_ROM_QSTR(MP_QSTR_SoftReset), MP_ROM_PTR(&machine_soft_reset_obj) },
#endif
#if MICROPY_QPY_MACHINE_RTC
	{ MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&machine_rtc_type) },
#endif
#if MICROPY_QPY_MACHINE_LCD
	{ MP_ROM_QSTR(MP_QSTR_LCD), MP_ROM_PTR(&machine_lcd_type) },
#endif
#if MICROPY_QPY_MACHINE_WDT
	{ MP_ROM_QSTR(MP_QSTR_WDT), MP_ROM_PTR(&machine_wdt_type) },
#endif
#if MICROPY_QPY_MACHINE_KEYPAD
	{ MP_ROM_QSTR(MP_QSTR_KeyPad), MP_ROM_PTR(&machine_keypad_type) },
#endif
#if MICROPY_QPY_MACHINE_NANDFLASH
	{ MP_ROM_QSTR(MP_QSTR_NANDFLASH), MP_ROM_PTR(&machine_nandflash_type) },
#endif
};

static MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);

#endif /* MICROPY_QPY_MODULE_MACHINE */

