/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "wm_include.h"
#include "wm_cpu.h"
#include "wm_watchdog.h"
#include "task.h"

#include "py/runtime.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_spi.h"

#include "modmachine.h"

#if MICROPY_PY_MACHINE

typedef enum {
    MP_PWRON_RESET = 0,
    MP_DEEPSLEEP_RESET
} reset_reason_t;

typedef enum {
    MP_PS_SLEEP = 0,
    MP_DEEP_SLEEP
} sleep_type_t;

typedef enum {
    MP_OP_PS_WAKEUP = 0,
    MP_OP_GPIO_WAKEUP = 0,
    MP_OP_PS_SLEEP = 1,
    MP_OP_TIMER_WAKEUP = 1,
} sleep_operate_t;

STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        tls_sys_clk sysclk;
        tls_sys_clk_get(&sysclk);
        return mp_obj_new_int(sysclk.cpuclk * 1000000);
    } else {
        // set
        mp_int_t freq = mp_obj_get_int(args[0]) / 1000000;
        if (freq != 40 && freq != 80) {
            mp_raise_ValueError("frequency can only be either 40MHz or 80MHz");
        }
        if (40 == freq) {
            tls_sys_clk_set(CPU_CLK_40M);
        } else {
            tls_sys_clk_set(CPU_CLK_80M);
        }
        tls_os_timer_init();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

STATIC mp_obj_t machine_sleep_helper(sleep_type_t sleep_type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if (n_args == 0) {
        mp_raise_ValueError("invalid param format");
    }

    enum {ARG_operate, ARG_sleep_s};
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_operate, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_sleep_s, MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t operate = args[ARG_operate].u_int;

    switch(sleep_type) {
    case MP_PS_SLEEP:
        if (n_args != 1)
            mp_raise_ValueError("invalid param format");
        tls_wl_if_ps(operate);//MP_OP_PS_WAKEUP,MP_OP_PS_SLEEP
        break;
    case MP_DEEP_SLEEP:
        if (n_args != 2)
            mp_raise_ValueError("invalid param format");
        mp_int_t sleeptime = args[ARG_sleep_s].u_int;
        tls_wl_if_standby(operate, 0, sleeptime);//MP_OP_GPIO_WAKEUP,MP_OP_TIMER_WAKEUP
        break;
    }
    return mp_const_none;
}

STATIC mp_obj_t machine_sleep(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return machine_sleep_helper(MP_PS_SLEEP, n_args, pos_args, kw_args);
};
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_sleep_obj, 0,  machine_sleep);

STATIC mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return machine_sleep_helper(MP_DEEP_SLEEP, n_args, pos_args, kw_args);
};
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_deepsleep_obj, 0,  machine_deepsleep);

STATIC mp_obj_t machine_reset_cause(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & (1 << 8))
        return MP_OBJ_NEW_SMALL_INT(MP_DEEPSLEEP_RESET);
    else
        return MP_OBJ_NEW_SMALL_INT(MP_PWRON_RESET);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_reset_cause_obj, 0,  machine_reset_cause);

STATIC mp_obj_t machine_wake_reason(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, "get wake reason not available for this version"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_wake_reason_obj, 0,  machine_wake_reason);

STATIC mp_obj_t machine_reset(void) {
    tls_sys_reset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_unique_id(void) {
    uint8_t *chipid;
    chipid = wpa_supplicant_get_mac();
    return mp_obj_new_bytes(chipid, 6);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_idle(void) {
    taskYIELD();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    MICROPY_END_ATOMIC_SECTION(state);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_umachine) },

    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&machine_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep), MP_ROM_PTR(&machine_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },

    { MP_ROM_QSTR(MP_QSTR_disable_irq), MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq), MP_ROM_PTR(&machine_enable_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_time_pulse_us), MP_ROM_PTR(&machine_time_pulse_us_obj) },

    { MP_ROM_QSTR(MP_QSTR_Signal), MP_ROM_PTR(&machine_signal_type) },

    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT), MP_ROM_PTR(&machine_wdt_type) },
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&machine_rtc_type) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&machine_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&mp_machine_soft_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&machine_adc_type) },

    //7816 i2s lcd dma..

    // wake abilities
    { MP_ROM_QSTR(MP_QSTR_PSWAKEUP), MP_ROM_INT(MP_OP_PS_WAKEUP) },
    { MP_ROM_QSTR(MP_QSTR_PSLEEP), MP_ROM_INT(MP_OP_PS_SLEEP) },
    { MP_ROM_QSTR(MP_QSTR_GPIO_WAKUP), MP_ROM_INT(MP_OP_GPIO_WAKEUP) },
    { MP_ROM_QSTR(MP_QSTR_TIMER_WAKUP), MP_ROM_INT(MP_OP_TIMER_WAKEUP) },

    // Reset reasons
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET), MP_ROM_INT(MP_PWRON_RESET) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET), MP_ROM_INT(MP_DEEPSLEEP_RESET) },

    // Wake reasons
    { MP_ROM_QSTR(MP_QSTR_wake_reason), MP_ROM_PTR(&machine_wake_reason_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *) &machine_module_globals,
};

#endif // MICROPY_PY_MACHINE

