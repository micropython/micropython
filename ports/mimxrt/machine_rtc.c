/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 * Copyright (c) 2021 "Robert Hammelrath" <robert@hammelrath.com>
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

#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared/runtime/mpirq.h"
#include "shared/timeutils/timeutils.h"
#include "extmod/modmachine.h"
#include "modmachine.h"
#include "ticks.h"
#include "fsl_snvs_lp.h"
#include "fsl_snvs_hp.h"

static mp_int_t timeout = 0;

void machine_rtc_alarm_clear_en(void) {
    SNVS_LP_SRTC_DisableInterrupts(SNVS, SNVS_LPCR_LPTA_EN_MASK);
    while (SNVS->LPCR & SNVS_LPCR_LPTA_EN_MASK) {
    }
}

void machine_rtc_alarm_set_en() {
    SNVS_LP_SRTC_EnableInterrupts(SNVS, SNVS_LPCR_LPTA_EN_MASK);
    while (!(SNVS->LPCR & SNVS_LPCR_LPTA_EN_MASK)) {
    }
}

void machine_rtc_alarm_off(bool clear) {
    machine_rtc_alarm_clear_en();
    #ifdef MIMXRT117x_SERIES
    DisableIRQ(SNVS_HP_NON_TZ_IRQn);
    #else
    DisableIRQ(SNVS_HP_WRAPPER_IRQn);
    #endif

    if (clear) {
        SNVS->LPTAR = 0;
        timeout = 0;
        SNVS->LPSR = SNVS_LPSR_LPTA_MASK;
    }
}

void machine_rtc_alarm_on() {
    #ifdef MIMXRT117x_SERIES
    EnableIRQ(SNVS_HP_NON_TZ_IRQn);
    #else
    EnableIRQ(SNVS_HP_WRAPPER_IRQn);
    #endif
    machine_rtc_alarm_set_en();
}

uint32_t machine_rtc_get_seconds() {
    uint32_t seconds = 0;
    uint32_t tmp = 0;

    // Do consecutive reads until value is correct
    do {
        seconds = tmp;
        tmp = (SNVS->LPSRTCMR << 17U);
        tmp |= (SNVS->LPSRTCLR >> 15U);
    } while (tmp != seconds);

    return seconds;
}

void machine_rtc_alarm_helper(int seconds, bool repeat) {
    machine_rtc_alarm_off(true);

    SNVS->LPTAR = machine_rtc_get_seconds() + seconds;

    if (repeat) {
        timeout = seconds;
    }

    machine_rtc_alarm_on();
}

typedef struct _machine_rtc_irq_obj_t {
    mp_irq_obj_t base;
} machine_rtc_irq_obj_t;

static mp_uint_t machine_rtc_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    new_trigger /= 1000;
    if (!new_trigger) {
        machine_rtc_alarm_off(true);
    } else {
        machine_rtc_alarm_helper(new_trigger, true);
    }
    return 0;
}

static mp_uint_t machine_rtc_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    return 0;
}

static const mp_irq_methods_t machine_rtc_irq_methods = {
    .trigger = machine_rtc_irq_trigger,
    .info = machine_rtc_irq_info,
};

void SNVS_HP_WRAPPER_IRQHandler(void) {
    if (SNVS->LPSR & SNVS_LPSR_LPTA_MASK) {
        SNVS->LPSR = SNVS_LPSR_LPTA_MASK;
        machine_rtc_irq_obj_t *irq = MP_STATE_PORT(machine_rtc_irq_object);
        if (irq != NULL) {
            mp_irq_handler(&irq->base);
        }
        if (timeout > 0) {
            machine_rtc_alarm_clear_en();
            SNVS->LPTAR += timeout;
            machine_rtc_alarm_set_en();
        }
    }
}

// Deinit rtc IRQ handler.
void machine_rtc_irq_deinit(void) {
    machine_rtc_alarm_off(true);
    MP_STATE_PORT(machine_rtc_irq_object) = NULL;
}

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// Singleton RTC object.
static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

// Start the RTC Timer.
void machine_rtc_start(void) {
    // Enable Non-Privileged Software Access
    SNVS->HPCOMR |= SNVS_HPCOMR_NPSWA_EN_MASK;
    // Do a basic init.
    SNVS_LP_Init(SNVS);
    #if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 0)
    // Disable all external Tamper
    SNVS_LP_DisableAllExternalTamper(SNVS);
    #endif

    SNVS_LP_SRTC_StartTimer(SNVS);
    // If the date is not set, set it to a more recent start date,
    // MicroPython's first commit.
    snvs_lp_srtc_datetime_t srtc_date;
    SNVS_LP_SRTC_GetDatetime(SNVS, &srtc_date);
    if (srtc_date.year <= 1970) {
        srtc_date = (snvs_lp_srtc_datetime_t) {
            .year = 2013,
            .month = 10,
            .day = 14,
            .hour = 19,
            .minute = 53,
            .second = 11,
        };
        SNVS_LP_SRTC_SetDatetime(SNVS, &srtc_date);
    }
}

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check arguments.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // Return constant object.
    return (mp_obj_t)&machine_rtc_obj;
}

static mp_obj_t machine_rtc_datetime_helper(size_t n_args, const mp_obj_t *args, int hour_index) {
    if (n_args == 1) {
        // Get date and time.
        snvs_lp_srtc_datetime_t srtc_date;
        SNVS_LP_SRTC_GetDatetime(SNVS, &srtc_date);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(srtc_date.year),
            mp_obj_new_int(srtc_date.month),
            mp_obj_new_int(srtc_date.day),
            mp_obj_new_int(timeutils_calc_weekday(srtc_date.year, srtc_date.month, srtc_date.day)),
            mp_obj_new_int(srtc_date.hour),
            mp_obj_new_int(srtc_date.minute),
            mp_obj_new_int(srtc_date.second),
            mp_obj_new_int(0),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set date and time.
        mp_obj_t *items;
        mp_int_t year;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        snvs_lp_srtc_datetime_t srtc_date;
        year = mp_obj_get_int(items[0]);
        srtc_date.year = year >= 100 ? year : year + 2000; // allow 21 for 2021
        srtc_date.month = mp_obj_get_int(items[1]);
        srtc_date.day = mp_obj_get_int(items[2]);
        // Ignore weekday at items[3]
        srtc_date.hour = mp_obj_get_int(items[hour_index]);
        srtc_date.minute = mp_obj_get_int(items[hour_index + 1]);
        srtc_date.second = mp_obj_get_int(items[hour_index + 2]);
        if (SNVS_LP_SRTC_SetDatetime(SNVS, &srtc_date) != kStatus_Success) {
            mp_raise_ValueError(NULL);
        }

        return mp_const_none;
    }
}

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args, 4);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    machine_rtc_datetime_helper(2, args, 3);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// calibration(cal)
// When the argument is a number in the range [-16 to 15], set the calibration value.
static mp_obj_t machine_rtc_calibration(mp_obj_t self_in, mp_obj_t cal_in) {
    mp_int_t cal = 0;
    snvs_lp_srtc_config_t snvsSrtcConfig;
    cal = mp_obj_get_int(cal_in);
    if (cal < -16 || cal > 15) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range -16 to 15"));
    } else {
        snvsSrtcConfig.srtcCalEnable = true;
        snvsSrtcConfig.srtcCalValue = cal & 0x1f; // 5 bit 2's complement
        SNVS_LP_SRTC_Init(SNVS, &snvsSrtcConfig);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_calibration_obj, machine_rtc_calibration);

static mp_obj_t machine_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,                            MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_time,                          MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_repeat,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // check the alarm id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    mp_int_t seconds = 0;
    snvs_lp_srtc_datetime_t srtc_date;
    bool repeat = args[2].u_bool;
    if (mp_obj_is_type(args[1].u_obj, &mp_type_tuple)) { // datetime tuple given
        // repeat cannot be used with a datetime tuple
        if (repeat) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
        }
        // Set date and time.
        mp_obj_t *items;
        mp_int_t year;
        mp_obj_get_array_fixed_n(args[1].u_obj, 8, &items);

        year = mp_obj_get_int(items[0]);
        srtc_date.year = year >= 100 ? year : year + 2000; // allow 21 for 2021
        srtc_date.month = mp_obj_get_int(items[1]);
        srtc_date.day = mp_obj_get_int(items[2]);
        // Ignore weekday at items[3]
        srtc_date.hour = mp_obj_get_int(items[4]);
        srtc_date.minute = mp_obj_get_int(items[5]);
        srtc_date.second = mp_obj_get_int(items[6]);
        machine_rtc_alarm_off(true);
        if (SNVS_LP_SRTC_SetAlarm(SNVS, &srtc_date) != kStatus_Success) {
            mp_raise_ValueError(NULL);
        }
        machine_rtc_alarm_on();
    } else { // then it must be an integer
        seconds = (args[1].u_obj == mp_const_none) ? 0 : (mp_obj_get_int(args[1].u_obj) / 1000);
        machine_rtc_alarm_helper(seconds, repeat);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

static mp_obj_t machine_rtc_alarm_left(size_t n_args, const mp_obj_t *args) {
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    uint32_t seconds = machine_rtc_get_seconds();
    uint32_t alarmSeconds = SNVS->LPTAR;
    return mp_obj_new_int((alarmSeconds >= seconds) ? ((alarmSeconds - seconds) * 1000) : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_left_obj, 1, 2, machine_rtc_alarm_left);

static mp_obj_t machine_rtc_alarm_cancel(size_t n_args, const mp_obj_t *args) {
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    machine_rtc_alarm_off(true);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_cancel_obj, 1, 2, machine_rtc_alarm_cancel);

static mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_handler, ARG_wake, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_wake, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // check we want alarm0
    if (args[ARG_trigger].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    machine_rtc_irq_obj_t *irq = MP_STATE_PORT(machine_rtc_irq_object);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_rtc_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_rtc_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(pos_args[0]);
        irq->base.handler = mp_const_none;
        irq->base.ishard = args[ARG_hard].u_bool;
        MP_STATE_PORT(machine_rtc_irq_object) = irq;
    }

    machine_rtc_alarm_off(false);

    irq->base.handler = args[ARG_handler].u_obj;

    if (SNVS->LPTAR) {
        machine_rtc_alarm_on();
    }

    return MP_OBJ_FROM_PTR(irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&machine_rtc_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm), MP_ROM_PTR(&machine_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_left), MP_ROM_PTR(&machine_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_cancel), MP_ROM_PTR(&machine_rtc_alarm_cancel_obj) },
    #if !MICROPY_PREVIEW_VERSION_2
    { MP_ROM_QSTR(MP_QSTR_cancel), MP_ROM_PTR(&machine_rtc_alarm_cancel_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_rtc_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_ALARM0), MP_ROM_INT(0) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );

MP_REGISTER_ROOT_POINTER(void *machine_rtc_irq_object);
