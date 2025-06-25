#include <Multiverse.h>
#include "py/obj.h"
#include "shared/timeutils/timeutils.h"
#include "macutil.h"

#define pyb_rtc_get_seconds() ({ \
        ULONGINT time; \
        GetDateTime(&time); \
        time; \
    })

// Return the localtime as an 8-tuple.
static mp_obj_t mp_time_localtime_get(void) {
    timeutils_struct_time_t tm;

    // get the seconds from the RTC
    timeutils_seconds_since_2000_to_struct_time(pyb_rtc_get_seconds(), &tm);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(tm.tm_year),
        mp_obj_new_int(tm.tm_mon),
        mp_obj_new_int(tm.tm_mday),
        mp_obj_new_int(tm.tm_hour),
        mp_obj_new_int(tm.tm_min),
        mp_obj_new_int(tm.tm_sec),
        mp_obj_new_int(tm.tm_wday),
        mp_obj_new_int(tm.tm_yday)
    };
    return mp_obj_new_tuple(8, tuple);
}

// Returns the number of seconds, as an integer, since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    return mp_obj_new_int(pyb_rtc_get_seconds());
}
// Returns the number of seconds, as an integer, since the Epoch.
// sadly I don't see how to get sub-second epoch time. There is a nominally
// 1/60s "tick" but that is keyed to the vertical retrace and is free running
// with respect to the 1s tick from the RTC. So, just return full seconds.
uint64_t mp_hal_time_ns(void) {
    return pyb_rtc_get_seconds() * 1000000000ull;
}
