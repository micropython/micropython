/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 "Krzysztof Adamski" <k@japko.eu>
 * Copyright (c) 2026 Infineon Technologies AG
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
#include <stdlib.h>
#include <inttypes.h>
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

// port-specific includes
#include "modmachine.h"

#include "cy_rtc.h"
#include "cybsp.h"

#define RTC_INIT_YEAR       15  /* 2015 = 2000 + 15 (RTC hardware stores 0-99) */
#define RTC_INIT_MONTH      1   /* January */
#define RTC_INIT_MDAY       1
#define RTC_INIT_WDAY       4   /* Thursday */
#define RTC_INIT_HOUR       0
#define RTC_INIT_MINUTE     0
#define RTC_INIT_SECOND     0
#define RTC_INIT_DST        0

#define TM_YEAR_BASE        (1900u)
#define RTC_CENTURY         (2000u)  /* RTC hardware supports years 0-99 = 2000-2099 */
#define RTC_ACCESS_RETRY_COUNT      (500U)
#define RTC_RETRY_DELAY_MS          (5U)

#define rtc_assert_raise(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_ValueError(MP_ERROR_TEXT(msg)); \
}

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
    mp_obj_t alarm_period_s;
    mp_obj_t alarm_elapse_time_s;
    bool alarmset;
    bool repeat;
} machine_rtc_obj_t;

// singleton RTC object
static machine_rtc_obj_t machine_rtc_obj = {
    .base = {&machine_rtc_type},
    .callback = NULL,
    .alarm_period_s = NULL,
    .alarm_elapse_time_s = NULL,
    .alarmset = false,
    .repeat = false
};

void machine_rtc_init_all(void) {
    /* Variable used to store return status of RTC API */
    cy_en_rtc_status_t rtc_status;
    uint32_t rtc_access_retry = RTC_ACCESS_RETRY_COUNT;

    /* RTC block doesn't allow to access, when synchronizing the user registers
    * and the internal actual RTC registers. It will return RTC_BUSY value, if
    * it is not available to update the configuration values. Needs to retry,
    * if it doesn't return CY_RTC_SUCCESS. */

    do
    {
        rtc_status = Cy_RTC_Init(&CYBSP_RTC_config);
        rtc_access_retry--;
        Cy_SysLib_Delay(RTC_RETRY_DELAY_MS);
    } while ((rtc_status != CY_RTC_SUCCESS) && (rtc_access_retry != 0));
}

// Helper function to set/get datetime
// Note: The weekday field is always auto-calculated by the RTC hardware based on the date.
// Even if an invalid weekday is provided in the input tuple, it will be recalculated and
// overwritten with the correct value. datetime() does not raise an error for incorrect
// weekday values - the hardware ensures weekday always matches the actual calendar date.
static mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    cy_stc_rtc_config_t curr_date_time = {0};  // Initialize all fields to 0
    if (n_args == 1) {
        /* Get the current date and time */
        Cy_RTC_GetDateAndTime(&curr_date_time);
        mp_obj_t tuple[8] = {
            mp_obj_new_int(curr_date_time.year + RTC_CENTURY),
            mp_obj_new_int(curr_date_time.month),
            mp_obj_new_int(curr_date_time.date),
            mp_obj_new_int(curr_date_time.dayOfWeek),
            mp_obj_new_int(curr_date_time.hour),
            mp_obj_new_int(curr_date_time.min),
            mp_obj_new_int(curr_date_time.sec),
            mp_obj_new_int(0),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set date and time using direct API
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        int year_full = mp_obj_get_int(items[0]);
        int month = mp_obj_get_int(items[1]);
        int date = mp_obj_get_int(items[2]);
        // items[3] is dayOfWeek - ignored and auto-calculated by RTC hardware
        // The hardware will compute the correct weekday from the year/month/date
        int hour = mp_obj_get_int(items[4]);
        int min = mp_obj_get_int(items[5]);
        int sec = mp_obj_get_int(items[6]);

        // Validate inputs
        if (year_full < RTC_CENTURY || year_full > RTC_CENTURY + 99) {
            mp_raise_ValueError(MP_ERROR_TEXT("year must be 2000-2099"));
        }
        if (month < 1 || month > 12) {
            mp_raise_ValueError(MP_ERROR_TEXT("month must be 1-12"));
        }
        if (date < 1 || date > 31) {
            mp_raise_ValueError(MP_ERROR_TEXT("date must be 1-31"));
        }
        if (hour > 23) {
            mp_raise_ValueError(MP_ERROR_TEXT("hour must be 0-23"));
        }
        if (min > 59) {
            mp_raise_ValueError(MP_ERROR_TEXT("minute must be 0-59"));
        }
        if (sec > 59) {
            mp_raise_ValueError(MP_ERROR_TEXT("second must be 0-59"));
        }

        uint32_t year = year_full - RTC_CENTURY;
        Cy_RTC_SetDateAndTimeDirect(sec, min, hour, date, month, year);
    }
    return mp_const_none;
}

// Helper to convert datetime tuple to seconds since epoch
static inline uint64_t rtc_get_datetime_in_sec(mp_obj_t datetime) {
    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(datetime, &len, &elem);

    // Expect tuple of length 8
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("datetime tuple needs length 8 or 9"));
    }

    // Format: (year, month, day, weekday, hour, minute, second, subseconds)
    return timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]),
        mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5]), mp_obj_get_int(elem[6]));
}

// Helper to get current time as seconds since epoch
static inline uint64_t rtc_get_current_time_in_sec() {
    mp_obj_t datetime = machine_rtc_datetime_helper(1, NULL);

    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(datetime, &len, &elem);

    // Expect tuple of length 8
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("datetime tuple needs length 8 or 9"));
    }

    // Format: (year, month, day, weekday, hour, minute, second, subseconds)
    // Note: elem[0] is already full year (2000-2099), not year since 1900
    return timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]),
        mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5]), mp_obj_get_int(elem[6]));
}


/* Forward declarations */
static void rtc_irq_handler(void *self_in, uint32_t event);
static void rtc_alarm_setup(machine_rtc_obj_t *self);

/* Override weak PDL alarm interrupt handler */
void Cy_RTC_Alarm1Interrupt(void) {
    /* Call our MicroPython callback handler */
    rtc_irq_handler(&machine_rtc_obj, 0);
}

/* Hardware RTC interrupt handler (called by NVIC) - must match IRQn naming */
void srss_interrupt_rtc_IRQHandler(void) {
    /* Use PDL middleware to dispatch to specific alarm/DST handlers */
    Cy_RTC_Interrupt(NULL, false);  /* NULL = no DST, false = normal mode */
}

/* Setup alarm hardware to trigger at the specified time */
static void rtc_alarm_setup(machine_rtc_obj_t *self) {
    if (self->alarm_elapse_time_s == NULL) {
        return;
    }

    /* Get the alarm time in seconds since epoch */
    uint64_t alarm_time_s = mp_obj_get_int(self->alarm_elapse_time_s);

    /* Convert seconds since epoch to datetime components */
    timeutils_struct_time_t tm;
    timeutils_seconds_since_epoch_to_struct_time(alarm_time_s, &tm);

    /* Set the alarm using PDL direct API (no year field in alarm, matches recurring time) */
    cy_en_rtc_status_t status = Cy_RTC_SetAlarmDateAndTimeDirect(
        tm.tm_sec, tm.tm_min, tm.tm_hour,
        tm.tm_mday, tm.tm_mon, CY_RTC_ALARM_1);

    if (status != CY_RTC_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to set alarm"));
    }

    /* Enable RTC ALARM1 interrupt */
    Cy_RTC_ClearInterrupt(CY_RTC_INTR_ALARM1);
    Cy_RTC_SetInterruptMask(CY_RTC_INTR_ALARM1);

    /* Enable NVIC interrupt for RTC */
    NVIC_SetPriority(srss_interrupt_rtc_IRQn, 3);
    NVIC_EnableIRQ(srss_interrupt_rtc_IRQn);
}

/* Software IRQ handler callback */
void rtc_irq_handler(void *self_in, uint32_t event) {
    machine_rtc_obj_t *self = (machine_rtc_obj_t *)self_in;
    if (self->callback != NULL) {
        /* Schedule callback to execute outside interrupt context */
        mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
    }
}


// MicroPython bindings for RTC class
static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return (mp_obj_t)&machine_rtc_obj;
}

static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t datetime) {
    machine_rtc_obj_t *self = (machine_rtc_obj_t *)self_in;
    self->alarm_elapse_time_s = NULL;
    self->alarm_period_s = NULL;
    self->alarmset = false;
    self->callback = NULL;
    self->repeat = false;

    mp_obj_t args[2] = {self_in, datetime};
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

static mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
    /* Resets RTC to 1st Jan' 2015 as mentioned in MPY guide*/
    Cy_RTC_SetDateAndTimeDirect(RTC_INIT_SECOND, RTC_INIT_MINUTE, RTC_INIT_HOUR,
        RTC_INIT_MDAY, RTC_INIT_MONTH, RTC_INIT_YEAR);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *datetime) {
    return machine_rtc_datetime_helper(n_args, datetime);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

static mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    return machine_rtc_datetime_helper(1, NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

static mp_obj_t machine_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_time, MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_repeat, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse args
    uint64_t dtime_sec;
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint64_t alarm_set_time_s = rtc_get_current_time_in_sec();

    self->repeat = args[1].u_bool;

    if (mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) { // datetime tuple given
        // repeat cannot be used with a datetime tuple
        if (self->repeat) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
        }
        dtime_sec = rtc_get_datetime_in_sec(args[0].u_obj);
        self->alarm_period_s = mp_obj_new_int_from_uint(dtime_sec - alarm_set_time_s);
    } else { // then it must be an integer
        self->alarm_period_s = mp_obj_new_int_from_uint(mp_obj_get_int(args[0].u_obj) / 1000);
    }
    self->alarm_elapse_time_s = mp_obj_new_int_from_uint(alarm_set_time_s + mp_obj_get_int(self->alarm_period_s));
    self->alarmset = true;

    /* Configure hardware alarm */
    rtc_alarm_setup(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

static mp_obj_t machine_rtc_alarm_left(size_t n_args, const mp_obj_t *args) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (self->alarmset) {
        uint64_t curr_time = rtc_get_current_time_in_sec();
        uint64_t alarm_time = mp_obj_get_int(self->alarm_elapse_time_s);
        return mp_obj_new_int_from_uint((alarm_time >= curr_time) ? ((alarm_time - curr_time) * 1000) : 0);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Alarm not set"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_left_obj, 1, 2, machine_rtc_alarm_left);

static mp_obj_t machine_rtc_alarm_cancel(size_t n_args, const mp_obj_t *args) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    /* Disable the hardware alarm */
    Cy_RTC_SetInterruptMask(0);  /* Disable all RTC interrupts */
    Cy_RTC_ClearInterrupt(CY_RTC_INTR_ALARM1);  /* Clear any pending */
    NVIC_DisableIRQ(srss_interrupt_rtc_IRQn);

    /* Clear software alarm state */
    self->alarmset = false;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_cancel_obj, 1, 2, machine_rtc_alarm_cancel);

static mp_obj_t machine_rtc_memory(size_t n_args, const mp_obj_t *args) {
    // For this implementation, memory() is an alias for datetime()
    // as the RTC hardware persists across soft resets
    return machine_rtc_datetime_helper(n_args, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_memory_obj, 1, 2, machine_rtc_memory);

static mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_handler, ARG_wake };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_handler, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_wake, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    self->callback = args[ARG_handler].u_obj;
    if (args[ARG_handler].u_obj == mp_const_none) {
        self->callback = NULL;
    }

    if (args[ARG_trigger].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    if (args[ARG_wake].u_int != -1) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("wake not implemented"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime),    MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now),         MP_ROM_PTR(&machine_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm),       MP_ROM_PTR(&machine_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_left),  MP_ROM_PTR(&machine_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_cancel),      MP_ROM_PTR(&machine_rtc_alarm_cancel_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),         MP_ROM_PTR(&machine_rtc_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_memory),      MP_ROM_PTR(&machine_rtc_memory_obj)},

};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
