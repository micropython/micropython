// modtime_port.c - port hooks for the `time` module
//
// extmod/modtime.c calls mp_time_time_get() when MICROPY_PY_TIME_TIME_TIME_NS
// is enabled.  We derive it from the same wall clock used for filesystem
// timestamps, so `time.time()` and `time.time_ns()` agree with file mtimes.

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#if MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME
#include "shared/timeutils/timeutils.h"
#endif

#if MICROPY_PY_TIME

mp_obj_t mp_time_time_get(void) {
    return mp_obj_new_int_from_ull(mp_hal_time_ns() / 1000000000ull);
}


#if MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME
// extmod/modtime.c calls this for time.localtime()/gmtime().  The port has no
// timezone database, so local time is UTC - the same choice other bare-metal
// ports make.  Derived from the same wall clock as time.time().
static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    timeutils_seconds_since_1970_to_struct_time(mp_hal_time_ns() / 1000000000ull, tm);
}
#endif
#endif
