/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

#include "xtirq.h"
#include "os_type.h"
#include "osapi.h"
#include "etshal.h"
#include "ets_alt_task.h"
#include "user_interface.h"

#if MICROPY_PY_MACHINE

//#define MACHINE_WAKE_IDLE (0x01)
//#define MACHINE_WAKE_SLEEP (0x02)
#define MACHINE_WAKE_DEEPSLEEP (0x04)

extern const mp_obj_type_t esp_wdt_type;

STATIC mp_obj_t machine_freq(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        return mp_obj_new_int(system_get_cpu_freq() * 1000000);
    } else {
        // set
        mp_int_t freq = mp_obj_get_int(args[0]) / 1000000;
        if (freq != 80 && freq != 160) {
            mp_raise_ValueError("frequency can only be either 80Mhz or 160MHz");
        }
        system_update_cpu_freq(freq);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

STATIC mp_obj_t machine_reset(void) {
    system_restart();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_reset_cause(void) {
    return MP_OBJ_NEW_SMALL_INT(system_get_rst_info()->reason);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC mp_obj_t machine_unique_id(void) {
    uint32_t id = system_get_chip_id();
    return mp_obj_new_bytes((byte*)&id, sizeof(id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_idle(void) {
    uint32_t t = mp_hal_ticks_cpu();
    asm("waiti 0");
    t = mp_hal_ticks_cpu() - t;
    return MP_OBJ_NEW_SMALL_INT(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC mp_obj_t machine_sleep(void) {
    printf("Warning: not yet implemented\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_sleep_obj, machine_sleep);

STATIC mp_obj_t machine_deepsleep(void) {
    // default to sleep forever
    uint32_t sleep_us = 0;

    // see if RTC.ALARM0 should wake the device
    if (pyb_rtc_alarm0_wake & MACHINE_WAKE_DEEPSLEEP) {
        uint64_t t = pyb_rtc_get_us_since_2000();
        if (pyb_rtc_alarm0_expiry <= t) {
            sleep_us = 1; // alarm already expired so wake immediately
        } else {
            uint64_t delta = pyb_rtc_alarm0_expiry - t;
            if (delta <= 0xffffffff) {
                // sleep for the desired time
                sleep_us = delta;
            } else {
                // overflow, just set to maximum sleep time
                sleep_us = 0xffffffff;
            }
        }
    }

    // prepare for RTC reset at wake up
    rtc_prepare_deepsleep(sleep_us);
    // put the device in a deep-sleep state
    system_deep_sleep_set_option(0); // default power down mode; TODO check this
    system_deep_sleep(sleep_us);

    for (;;) {
        // we must not return
        ets_loop_iter();
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_deepsleep_obj, machine_deepsleep);

typedef struct _esp_timer_obj_t {
    mp_obj_base_t base;
    os_timer_t timer;
    mp_obj_t callback;
} esp_timer_obj_t;

const mp_obj_type_t esp_timer_type;

STATIC void esp_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp_timer_obj_t *self = self_in;
    mp_printf(print, "Timer(%p)", &self->timer);
}

STATIC mp_obj_t esp_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    esp_timer_obj_t *tim = m_new_obj(esp_timer_obj_t);
    tim->base.type = &esp_timer_type;
    return tim;
}

STATIC void esp_timer_cb(void *arg) {
    esp_timer_obj_t *self = arg;
    mp_sched_schedule(self->callback, self);
}

STATIC mp_obj_t esp_timer_init_helper(esp_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
//        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->callback = args[2].u_obj;
    // Be sure to disarm timer before making any changes
    os_timer_disarm(&self->timer);
    os_timer_setfn(&self->timer, esp_timer_cb, self);
    os_timer_arm(&self->timer, args[0].u_int, args[1].u_int);

    return mp_const_none;
}

STATIC mp_obj_t esp_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return esp_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp_timer_init_obj, 1, esp_timer_init);

STATIC mp_obj_t esp_timer_deinit(mp_obj_t self_in) {
    esp_timer_obj_t *self = self_in;
    os_timer_disarm(&self->timer);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_timer_deinit_obj, esp_timer_deinit);

STATIC const mp_rom_map_elem_t esp_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&esp_timer_init_obj) },
//    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&esp_timer_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(false) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(true) },
};
STATIC MP_DEFINE_CONST_DICT(esp_timer_locals_dict, esp_timer_locals_dict_table);

const mp_obj_type_t esp_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = esp_timer_print,
    .make_new = esp_timer_make_new,
    .locals_dict = (mp_obj_dict_t*)&esp_timer_locals_dict,
};

// this bit is unused in the Xtensa PS register
#define ETS_LOOP_ITER_BIT (12)

STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = disable_irq();
    state = (state & ~(1 << ETS_LOOP_ITER_BIT)) | (ets_loop_iter_disable << ETS_LOOP_ITER_BIT);
    ets_loop_iter_disable = 1;
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    ets_loop_iter_disable = (state >> ETS_LOOP_ITER_BIT) & 1;
    enable_irq(state & ~(1 << ETS_LOOP_ITER_BIT));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },

    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&machine_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep), MP_ROM_PTR(&machine_deepsleep_obj) },

    { MP_ROM_QSTR(MP_QSTR_disable_irq), MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq), MP_ROM_PTR(&machine_enable_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_time_pulse_us), MP_ROM_PTR(&machine_time_pulse_us_obj) },

    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&pyb_rtc_type) },
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&esp_timer_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT), MP_ROM_PTR(&esp_wdt_type) },
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&pyb_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_Signal), MP_ROM_PTR(&machine_signal_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&pyb_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&pyb_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&pyb_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_hspi_type) },

    // wake abilities
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP), MP_ROM_INT(MACHINE_WAKE_DEEPSLEEP) },

    // reset causes
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET), MP_ROM_INT(REASON_DEFAULT_RST) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET), MP_ROM_INT(REASON_EXT_SYS_RST) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET), MP_ROM_INT(REASON_DEEP_SLEEP_AWAKE) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET), MP_ROM_INT(REASON_WDT_RST) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET), MP_ROM_INT(REASON_SOFT_RESTART) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

#endif // MICROPY_PY_MACHINE
