/*
 * astrorafael port of RP2 Real Time Clock module, based on previous
 * work by the MicroPython project
 *
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

// --------------------
// MicroPython includes
// --------------------

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

// ----------------------
// Pico specific includes
// ----------------------

#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// ------------------
// Exported constants
// ------------------

#define RTC_ALARM0 (0)

// ------------------
// Internal constants
// ------------------

/* 
 * This should be defined and exported in the Python machine module
 * but apparently it is not used
 */
#define RP2_PWR_MODE_IDLE         (0x01)
#define RP2_PWR_MODE_SLEEP        (0x02)
#define RP2_PWR_MODE_DEEPSLEEP    (0x04)


// From https://pdc.ro.nu/jd-code.html by Robin O'Leary
STATIC uint32_t gregorian_calendar_to_jd(uint32_t y, uint32_t m, uint32_t d) {
    y += 8000;
    if (m<3) { 
        y--; 
        m+=12; 
    }
    return (y*365) + (y/4) -(y/100) +(y/400) -1200820 + (m*153+3)/5 - 92 + d - 1;
}

// ----------------------------------------
// RTC internal state
// will become part of the Python RTC class
// ----------------------------------------

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} machine_rtc_obj_t;

const mp_obj_type_t machine_rtc_type;   // Forward declaration

// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

// ----------------------------------------
// class machine.RTC(id=0, ...) Constructor
// ----------------------------------------

mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get RTC id. Only 0 is supported
    int rtc_id = mp_obj_get_int(args[ARG_id].u_obj);

     if (rtc_id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("RTC(%d) doesn't exist"), rtc_id);
    }

    return (mp_obj_t)&machine_rtc_obj;
}

// -------------------------
// RTC.init(datetime) method
// -------------------------

/* 
 * Initialise the RTC. Datetime is a tuple of the form:
 * (year, month, day[, hour[, minute[, second[, microsecond[, tzinfo]]]]])
 * Warning: no support for microseconds or TZ info
 */


STATIC mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t datetime) {
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(datetime, 8, &items);
    datetime_t t = {
            .year  = mp_obj_get_int(items[0]),
            .month = mp_obj_get_int(items[1]),
            .day   = mp_obj_get_int(items[2]),  
            .hour  = mp_obj_get_int(items[3]),
            .min   = mp_obj_get_int(items[4]),
            .sec   = mp_obj_get_int(items[5]),
            .dotw  = 0, // 0 is Sunday, so 5 is Frida WHAT DO WE DO HERE ????
    };
    /*
     * Note that the Raspberry Pi Pico port starst the RTC hardware at boot time
     * so we do not start the hardware.
     */
#if 0  
    if ( ! rtc_running() ) {
        rtc_init();
    }
#endif
    rtc_set_datetime(&t);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// -------------------
// RTC.deinit() method
// -------------------

/* 
 * As per the MycroPython documantation:
 * "Resets the RTC to the time of January 1, 2015 and starts running it again."
 *
 * Note that the raspberry Pi Pico port starst the RTC hardware at micropython boot
 * so we do not stop the hardware.
 */

STATIC mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    datetime_t t = {
            .year  = 2015,
            .month = 1,
            .day   = 1,  
            .hour  = 0,
            .min   = 0,
            .sec   = 0,
            .dotw  = 4, // 0 is Sunday, so 4 is Thursday 
    };
    rtc_set_datetime(&t);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);

// ----------------
// RTC.now() method
// ----------------

STATIC mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    datetime_t t;
    rtc_get_datetime(&t);

    mp_obj_t tuple[8] = {
        mp_obj_new_int(t.year),
        mp_obj_new_int(t.month),
        mp_obj_new_int(t.day),
        mp_obj_new_int(t.hour),
        mp_obj_new_int(t.min),
        mp_obj_new_int(t.sec),
        mp_obj_new_int(0),
        mp_const_none
    };
    return mp_obj_new_tuple(8, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);


// -------------------------------------------
// RTC.alarm(id, time, *, repeat=False) method
// -------------------------------------------

STATIC mp_obj_t machine_rtc_alarm(mp_obj_t self_in) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_raise_NotImplementedError(MP_ERROR_TEXT("alarm()"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_alarm_obj, machine_rtc_alarm);

// ----------------------------------
// RTC.alarm_left(alarm_id=0)  method
// ----------------------------------

STATIC mp_obj_t machine_rtc_alarm_left(mp_obj_t self_in) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_raise_NotImplementedError(MP_ERROR_TEXT("alarm_left()"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_alarm_left_obj, machine_rtc_alarm_left);

// -----------------------------
// RTC.cancel(alarm_id=0) method
// -----------------------------

STATIC mp_obj_t machine_rtc_cancel(mp_obj_t self_in) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_raise_NotImplementedError(MP_ERROR_TEXT("cancel()"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_cancel_obj, machine_rtc_cancel);

// -----------------------------------------------------------
// RTC.irq(*, trigger, handler=None, wake=machine.IDLE) method
// -----------------------------------------------------------

STATIC mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_handler, ARG_wake };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = RTC_ALARM0} },
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_wake,    MP_ARG_INT, {.u_int = RP2_PWR_MODE_IDLE} },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]); 

    mp_raise_NotImplementedError(MP_ERROR_TEXT("irq()"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);




// -------------------------------
// RTC __repr__ && __str__ methods
// -------------------------------

STATIC void machine_rtc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    //machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "RTC(running = %s)", rtc_running() ? "true" : "false");
}

// -------------------------------------------------------------------------
// RTC locals dictionary, mappings names to additional methods and constants
// -------------------------------------------------------------------------

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    // Map Python methods
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_rtc_init_obj)   },
    { MP_ROM_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_now),     MP_ROM_PTR(&machine_rtc_now_obj)    },

    { MP_ROM_QSTR(MP_QSTR_alarm),      MP_ROM_PTR(&machine_rtc_alarm_obj)      },
    { MP_ROM_QSTR(MP_QSTR_alarm_left), MP_ROM_PTR(&machine_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_cancel),     MP_ROM_PTR(&machine_rtc_cancel_obj)     },
    { MP_ROM_QSTR(MP_QSTR_irq),        MP_ROM_PTR(&machine_rtc_irq_obj)        },

    // Map Python Constants
    { MP_ROM_QSTR(MP_QSTR_ALARM0),  MP_ROM_INT(RTC_ALARM0) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

// --------------------
// RTC Class definition
// --------------------

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .print = machine_rtc_print,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_rtc_locals_dict,
};
