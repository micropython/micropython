// machine_rtc_port.c - machine.RTC for GD32F4xx
// RTC has no extmod layer; this file defines the type.

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objtuple.h"

#include "gd32f4xx.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// forward declaration of the type (defined at the bottom)
extern const mp_obj_type_t machine_rtc_type;

static machine_rtc_obj_t machine_rtc_obj = {
    .base = { &machine_rtc_type },
};

// ---- BCD helpers ----
static uint8_t bcd2dec(uint8_t v) {
    return (uint8_t)((v >> 4) * 10 + (v & 0x0f));
}

static uint8_t dec2bcd(uint8_t v) {
    return (uint8_t)(((v / 10) << 4) | (v % 10));
}

// ---- RTC clock / domain setup ----
static void rtc_prepare_domain(void) {
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
    // backup interface clock is enabled via PMU on GD32F4
    rcu_periph_clock_enable(RCU_RTC);
    rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
    rcu_periph_clock_enable(RCU_IRC32K);
    rtc_register_sync_wait();
}

// ---- init([datetime]) / init(year, month, day[, wday, hour, min, sec, us]) ----
static mp_obj_t machine_rtc_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rtc_prepare_domain();

    rtc_parameter_struct p;
    p.factor_asyn = 0x7F;    // 32.768 kHz / (127+1) = 256 Hz
    p.factor_syn = 0xFF;     // 256 / (255+1) = 1 Hz
    p.am_pm = RTC_AM;
    p.display_format = RTC_24HOUR;
    p.day_of_week = 1;

    if (n_args >= 2 && mp_obj_is_type(pos_args[1], &mp_type_tuple)) {
        // init((y, m, d, wday, h, min, s, us))
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(pos_args[1], &len, &items);
        if (len < 8) {
            mp_raise_ValueError(MP_ERROR_TEXT("datetime needs 8 entries"));
        }
        p.year   = dec2bcd((uint8_t)(mp_obj_get_int(items[0]) % 100));
        p.month  = dec2bcd((uint8_t)mp_obj_get_int(items[1]));
        p.date   = dec2bcd((uint8_t)mp_obj_get_int(items[2]));
        p.day_of_week = (uint8_t)(mp_obj_get_int(items[3]) + 1);
        p.hour   = dec2bcd((uint8_t)mp_obj_get_int(items[4]));
        p.minute = dec2bcd((uint8_t)mp_obj_get_int(items[5]));
        p.second = dec2bcd((uint8_t)mp_obj_get_int(items[6]));
    } else if (n_args >= 4) {
        // init(year, month, day, wday, hour, minute, second, microsecond)
        p.year   = dec2bcd((uint8_t)(mp_obj_get_int(pos_args[1]) % 100));
        p.month  = dec2bcd((uint8_t)mp_obj_get_int(pos_args[2]));
        p.date   = dec2bcd((uint8_t)mp_obj_get_int(pos_args[3]));
        p.day_of_week = (n_args > 4) ? (uint8_t)(mp_obj_get_int(pos_args[4]) + 1) : 1;
        p.hour   = (n_args > 5) ? dec2bcd((uint8_t)mp_obj_get_int(pos_args[5])) : 0;
        p.minute = (n_args > 6) ? dec2bcd((uint8_t)mp_obj_get_int(pos_args[6])) : 0;
        p.second = (n_args > 7) ? dec2bcd((uint8_t)mp_obj_get_int(pos_args[7])) : 0;
    } else {
        // no datetime given: just ensure the domain is running
        (void)kw_args;
        return mp_const_none;
    }

    rtc_init(&p);
    rtc_register_sync_wait();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_init_obj, 1, machine_rtc_init);

// ---- datetime() -> (year, month, day, wday, hour, minute, second, 0) ----
static mp_obj_t machine_rtc_datetime(mp_obj_t self_in) {
    rtc_parameter_struct p;
    rtc_register_sync_wait();
    rtc_current_time_get(&p);

    mp_obj_t items[8] = {
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.year) + 2000),
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.month)),
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.date)),
        MP_OBJ_NEW_SMALL_INT(p.day_of_week - 1),
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.hour)),
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.minute)),
        MP_OBJ_NEW_SMALL_INT(bcd2dec(p.second)),
        MP_OBJ_NEW_SMALL_INT(0),
    };
    return mp_obj_new_tuple(8, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_datetime_obj, machine_rtc_datetime);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),     MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_ALARM0),   MP_ROM_INT(0) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    (void)type;
    machine_rtc_obj.base.type = (mp_obj_type_t *)&machine_rtc_type;
    rtc_prepare_domain();
    return MP_OBJ_FROM_PTR(&machine_rtc_obj);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );

// ---- epoch helper for filesystem timestamps ----
// Simple days-since-epoch calculation from the calendar RTC.
uint64_t machine_rtc_get_epoch_seconds(void) {
    rtc_parameter_struct p;
    rtc_register_sync_wait();
    rtc_current_time_get(&p);

    int year = 2000 + bcd2dec(p.year);
    int month = bcd2dec(p.month);
    int day = bcd2dec(p.date);
    int hour = bcd2dec(p.hour);
    int minute = bcd2dec(p.minute);
    int second = bcd2dec(p.second);

    // days since 1970-01-01
    static const int mdays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    uint32_t days = 0;
    for (int y = 1970; y < year; y++) {
        days += ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 366 : 365;
    }
    days += mdays[(month >= 1 && month <= 12) ? month - 1 : 0];
    if (month > 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        days += 1;
    }
    days += (uint32_t)(day - 1);

    return (uint64_t)days * 86400ull + (uint64_t)hour * 3600ull
         + (uint64_t)minute * 60ull + (uint64_t)second;
}
