/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 "Krzysztof Adamski" <k@japko.eu>
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

// std includes
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


// micropython includes
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"
#include "py/misc.h"


// MTB includes
#include "cyhal.h"


// port-specific includes
#include "modmachine.h"

/* The values are reflected to match MPY reset values*/
#define RTC_INIT_YEAR       2015
#define RTC_INIT_MONTH      1   /* January */
#define RTC_INIT_MDAY       1
#define RTC_INIT_WDAY       4   /* Thursday */
#define RTC_INIT_HOUR       0
#define RTC_INIT_MINUTE     0
#define RTC_INIT_SECOND     0
#define RTC_INIT_DST        0
#define TM_YEAR_BASE        (1900u)

cyhal_rtc_t psoc6_rtc;


typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    uint32_t alarm_ms;
    mp_obj_t callback;
    bool repeat;
} machine_rtc_obj_t;


// singleton RTC object
static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

/* This function is run from main.c to init the RTC at boot time. This will set the RTC to PSoC default time: 1st Jan 2000*/
void rtc_init(void) {
    /*cy_rslt_t result = cyhal_rtc_init(&psoc6_rtc);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_init failed !"));
    }*/
}

// Machine RTC methods - port-specific definitions
// RTC constructor
static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

// Helper function to set/get datetime
static mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        struct tm current_date_time = {0};
        cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);
        if (CY_RSLT_SUCCESS != result) {
            mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
        }

        mp_obj_t tuple[8] = {
            mp_obj_new_int(current_date_time.tm_year + TM_YEAR_BASE),
            mp_obj_new_int(current_date_time.tm_mon + 1),
            mp_obj_new_int(current_date_time.tm_mday),
            mp_obj_new_int(current_date_time.tm_wday),
            mp_obj_new_int(current_date_time.tm_hour),
            mp_obj_new_int(current_date_time.tm_min),
            mp_obj_new_int(current_date_time.tm_sec),
            mp_obj_new_int(0)
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1], 8, &items);

        struct tm current_date_time =
        {
            .tm_year = mp_obj_get_int(items[0]) - TM_YEAR_BASE,
            .tm_mon = mp_obj_get_int(items[1]) - 1,
            .tm_mday = mp_obj_get_int(items[2]),
            .tm_wday = mp_obj_get_int(items[3]),
            .tm_hour = mp_obj_get_int(items[4]),
            .tm_min = mp_obj_get_int(items[5]),
            .tm_sec = mp_obj_get_int(items[6]),
        };

        cy_rslt_t result = cyhal_rtc_write(&psoc6_rtc, &current_date_time);

        if (CY_RSLT_SUCCESS != result) {
            mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_write failed ! Check if field values entered are within the specified range."));
        }
    }
    return mp_const_none;
}

// RTC.datetime([datetime])
static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

// RTC.init(datetime)
static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    // Check if RTC is correctly initialized already through main
    bool r = cyhal_rtc_is_enabled(&psoc6_rtc);
    if (!r) {
        rtc_init();
    }
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// RTC.deinit()
static mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
    /* Resets RTC to 1st Jan' 2015 as mentioned in MPY guide*/
    struct tm reset_date_time = {
        .tm_year = RTC_INIT_YEAR - TM_YEAR_BASE,
        .tm_mon = RTC_INIT_MONTH - 1,
        .tm_mday = RTC_INIT_MDAY,
        .tm_wday = RTC_INIT_WDAY,
        .tm_hour = RTC_INIT_HOUR,
        .tm_min = RTC_INIT_MINUTE,
        .tm_sec = RTC_INIT_SECOND,
        .tm_isdst = RTC_INIT_DST
    };
    cy_rslt_t result = cyhal_rtc_write(&psoc6_rtc, &reset_date_time);
    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_write failed during RTC deinitialization!"));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);

// RTC.now()
static mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    return machine_rtc_datetime_helper(1, NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

static inline void rtc_get_dtime_struct(const mp_obj_t datetime, struct tm dtime) {
    // set date and time
    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(datetime, &len, &items);

    dtime.tm_sec = mp_obj_get_int(items[6]);
    dtime.tm_min = mp_obj_get_int(items[5]);
    dtime.tm_hour = mp_obj_get_int(items[4]);
    dtime.tm_mday = mp_obj_get_int(items[2]);
    dtime.tm_mon = mp_obj_get_int(items[1]) - 1;
    dtime.tm_year = mp_obj_get_int(items[0]) - TM_YEAR_BASE;
    dtime.tm_wday = mp_obj_get_int(items[3]);
    dtime.tm_isdst = 0;
}

static mp_obj_t machine_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    printf("\nAlarm function \r\n");
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,                            MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_time,                          MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_repeat,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    machine_rtc_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // check the alarm id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // cy_rslt_t result;
    bool repeat = args[2].u_bool;
    printf("We should be here \r\n");
    if (mp_obj_is_type(args[1].u_obj, &mp_type_tuple)) { // datetime tuple given
        // repeat cannot be used with a datetime tuple
        printf("Or \r\n");
        if (repeat) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
        }
        struct tm dtime;
        cyhal_alarm_active_t alarm_active =
        {
            .en_sec = 1,
            .en_min = 1,
            .en_hour = 1,
            .en_day = 1,
            .en_date = 1,
            .en_month = 1,
        };

        rtc_get_dtime_struct(args[1].u_obj, dtime);
        cyhal_rtc_set_alarm(&psoc6_rtc, &dtime, alarm_active);
    } else { // then it must be an integer
        printf("Inside here \r\n");
        // self->alarm_ms = mp_obj_get_int(args[1].u_obj);

        printf("Set alarm value: %ld\r\n", self->alarm_ms);
        cyhal_rtc_set_alarm_by_seconds(&psoc6_rtc, 1);
    }

    // store the repepat flag
    self->repeat = repeat;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

static void rtc_irq_handler(void *callback_arg, cyhal_rtc_event_t event) {
    machine_rtc_obj_t *self = callback_arg;
    mp_sched_schedule(self->callback, mp_obj_new_int(event));
}


void cyhal_rtc_alarm_interrupt_handler(void *arg, cyhal_rtc_event_t event) {
    (void)arg;
    if (event == CYHAL_RTC_ALARM) {
        printf("ALarm fired \r\n");
    }
}

#define RTC_CALLBACK_ARG NULL
static mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    printf("IRQ function\r\n");
    enum { ARG_trigger, ARG_handler, ARG_wake};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CYHAL_RTC_ALARM} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_wake, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} }
    };
    printf("CYHAL_RTC_ALARM: %d\r\n", CYHAL_RTC_ALARM);

    // machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // self->callback = args[MP_QSTR_handler].u_obj;
    cyhal_rtc_init(&psoc6_rtc);
    printf("setting alarm\r\n");
    cyhal_rtc_register_callback(&psoc6_rtc,
        (cyhal_rtc_event_callback_t)rtc_irq_handler,
        RTC_CALLBACK_ARG);
    cyhal_rtc_enable_event(&psoc6_rtc, CYHAL_RTC_ALARM, 3u, true);
    cy_rslt_t result = cyhal_rtc_set_alarm_by_seconds(&psoc6_rtc, 0);

    printf("Reached here and result is: %ld \r\n", result);
    // cyhal_system_delay_ms(1200);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);


static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&machine_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm),           MP_ROM_PTR(&machine_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_rtc_irq_obj) },
    // class constants
    { MP_ROM_QSTR(MP_QSTR_ALARM0),          MP_ROM_INT(CYHAL_RTC_ALARM) },

};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
