// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Wall-clock time for the time module. Included into extmod/modtime.c via
// MICROPY_PY_TIME_INCLUDEFILE (not compiled standalone). Provides time(), time_ns(),
// and gmtime/localtime/mktime via shared/timeutils. All three views come from the one
// anchored wall clock (mp_uefi_wall_ns, uefi_time.c): the RTC seeds the anchor and the
// counter interpolates, so seconds and sub-seconds are always mutually coherent.

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"
#include "uefi_port.h"

static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    timeutils_seconds_since_epoch_to_struct_time((mp_timestamp_t)(mp_uefi_wall_ns() / 1000000000ULL), tm);
}

static mp_obj_t mp_time_time_get(void) {
    return mp_obj_new_int_from_ull((unsigned long long)(mp_uefi_wall_ns() / 1000000000ULL));
}

uint64_t mp_hal_time_ns(void) {
    return mp_uefi_wall_ns();
}
