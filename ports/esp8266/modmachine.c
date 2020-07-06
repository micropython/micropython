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
#include "lib/utils/pyexec.h"

// This needs to be set before we include the RTOS headers
#define USE_US_TIMER 1

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

// #define MACHINE_WAKE_IDLE (0x01)
// #define MACHINE_WAKE_SLEEP (0x02)
#define MACHINE_WAKE_DEEPSLEEP (0x04)

extern const mp_obj_type_t esp_wdt_type;

STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        return mp_obj_new_int(system_get_cpu_freq() * 1000000);
    } else {
        // set
        mp_int_t freq = mp_obj_get_int(args[0]) / 1000000;
        if (freq != 80 && freq != 160) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency can only be either 80Mhz or 160MHz"));
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

STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    mp_raise_type(&mp_type_SystemExit);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

STATIC mp_obj_t machine_reset_cause(void) {
    return MP_OBJ_NEW_SMALL_INT(system_get_rst_info()->reason);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC mp_obj_t machine_unique_id(void) {
    uint32_t id = system_get_chip_id();
    return mp_obj_new_bytes((byte *)&id, sizeof(id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_idle(void) {
    uint32_t t = mp_hal_ticks_cpu();
    asm ("waiti 0");
    t = mp_hal_ticks_cpu() - t;
    ets_event_poll(); // handle any events after possibly a long wait (eg feed WDT)
    return MP_OBJ_NEW_SMALL_INT(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC mp_obj_t machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    uint32_t max_us = 0xffffffff;
    if (n_args == 1) {
        mp_int_t max_ms = mp_obj_get_int(args[0]);
        if (max_ms < 0) {
            max_ms = 0;
        }
        max_us = max_ms * 1000;
    }
    uint32_t wifi_mode = wifi_get_opmode();
    uint32_t start = system_get_time();
    while (system_get_time() - start <= max_us) {
        ets_event_poll();
        if (wifi_mode == NULL_MODE) {
            // Can only idle if the wifi is off
            asm ("waiti 0");
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lightsleep_obj, 0, 1, machine_lightsleep);

STATIC mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *args) {
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

    // if an argument is given then that's the maximum time to sleep for
    if (n_args == 1) {
        mp_int_t max_ms = mp_obj_get_int(args[0]);
        if (max_ms <= 0) {
            max_ms = 1;
        }
        uint32_t max_us = max_ms * 1000;
        if (sleep_us == 0 || max_us < sleep_us) {
            sleep_us = max_us;
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj, 0, 1, machine_deepsleep);

// These values are from the datasheet
#define ESP_TIMER_US_MIN (100)
#define ESP_TIMER_US_MAX (0xfffffff)
#define ESP_TIMER_MS_MAX (0x689d0)

typedef struct _esp_timer_obj_t {
    mp_obj_base_t base;
    os_timer_t timer;
    uint32_t remain_ms; // if non-zero, remaining time to handle large periods
    uint32_t period_ms; // if non-zero, periodic timer with a large period
    mp_obj_t callback;
} esp_timer_obj_t;

STATIC void esp_timer_arm_ms(esp_timer_obj_t *self, uint32_t ms, bool repeat) {
    if (ms <= ESP_TIMER_MS_MAX) {
        self->remain_ms = 0;
        self->period_ms = 0;
    } else {
        self->remain_ms = ms - ESP_TIMER_MS_MAX;
        if (repeat) {
            repeat = false;
            self->period_ms = ms;
        } else {
            self->period_ms = 0;
        }
        ms = ESP_TIMER_MS_MAX;
    }
    os_timer_arm(&self->timer, ms, repeat);
}

STATIC void esp_timer_arm_us(esp_timer_obj_t *self, uint32_t us, bool repeat) {
    if (us < ESP_TIMER_US_MIN) {
        us = ESP_TIMER_US_MIN;
    }
    if (us <= ESP_TIMER_US_MAX) {
        self->remain_ms = 0;
        self->period_ms = 0;
        os_timer_arm_us(&self->timer, us, repeat);
    } else {
        esp_timer_arm_ms(self, us / 1000, repeat);
    }
}

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
    if (self->remain_ms != 0) {
        // Handle periods larger than the maximum system period
        uint32_t next_period_ms = self->remain_ms;
        if (next_period_ms > ESP_TIMER_MS_MAX) {
            next_period_ms = ESP_TIMER_MS_MAX;
        }
        self->remain_ms -= next_period_ms;
        os_timer_arm(&self->timer, next_period_ms, false);
    } else {
        mp_sched_schedule(self->callback, self);
        if (self->period_ms != 0) {
            // A periodic timer with a larger period: reschedule it
            esp_timer_arm_ms(self, self->period_ms, true);
        }
    }
}

STATIC mp_obj_t esp_timer_init_helper(esp_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
        ARG_tick_hz,
        ARG_freq,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        #if MICROPY_PY_BUILTINS_FLOAT
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        #else
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #endif
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->callback = args[ARG_callback].u_obj;
    // Be sure to disarm timer before making any changes
    os_timer_disarm(&self->timer);
    os_timer_setfn(&self->timer, esp_timer_cb, self);

    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        mp_float_t freq = mp_obj_get_float(args[ARG_freq].u_obj);
        if (freq < 0.001) {
            esp_timer_arm_ms(self, (mp_int_t)(1000 / freq), args[ARG_mode].u_int);
        } else {
            esp_timer_arm_us(self, (mp_int_t)(1000000 / freq), args[ARG_mode].u_int);
        }
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        esp_timer_arm_us(self, 1000000 / args[ARG_freq].u_int, args[ARG_mode].u_int);
    }
    #endif
    else {
        mp_int_t period = args[ARG_period].u_int;
        mp_int_t hz = args[ARG_tick_hz].u_int;
        if (hz == 1000) {
            esp_timer_arm_ms(self, period, args[ARG_mode].u_int);
        } else if (hz == 1000000) {
            esp_timer_arm_us(self, period, args[ARG_mode].u_int);
        } else {
            // Use a long long to ensure that we don't either overflow or loose accuracy
            uint64_t period_us = (((uint64_t)period) * 1000000) / hz;
            if (period_us < 0x80000000ull) {
                esp_timer_arm_us(self, (mp_int_t)period_us, args[ARG_mode].u_int);
            } else {
                esp_timer_arm_ms(self, (mp_int_t)(period_us / 1000), args[ARG_mode].u_int);
            }
        }
    }

    return mp_const_none;
}

STATIC mp_obj_t esp_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
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
    .locals_dict = (mp_obj_dict_t *)&esp_timer_locals_dict,
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

// Custom version of this function that feeds system WDT if necessary
mp_uint_t machine_time_pulse_us(mp_hal_pin_obj_t pin, int pulse_level, mp_uint_t timeout_us) {
    int nchanges = 2;
    uint32_t start = system_get_time(); // in microseconds
    for (;;) {
        uint32_t dt = system_get_time() - start;

        // Check if pin changed to wanted value
        if (mp_hal_pin_read(pin) == pulse_level) {
            if (--nchanges == 0) {
                return dt;
            }
            pulse_level = 1 - pulse_level;
            start = system_get_time();
            continue;
        }

        // Check for timeout
        if (dt >= timeout_us) {
            return (mp_uint_t)-nchanges;
        }

        // Only feed WDT every now and then, to make sure edge timing is accurate
        if ((dt & 0xffff) == 0xffff && !ets_loop_dont_feed_sw_wdt) {
            system_soft_wdt_feed();
        }
    }
}

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },

    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&machine_lightsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_lightsleep), MP_ROM_PTR(&machine_lightsleep_obj) },
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
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&machine_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&pyb_uart_type) },
    #if MICROPY_PY_MACHINE_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
    #endif
    #if MICROPY_PY_MACHINE_SPI
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_hspi_type) },
    #endif

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
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

#endif // MICROPY_PY_MACHINE
