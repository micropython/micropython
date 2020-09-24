#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H

#include "py/obj.h"

#include "shared-bindings/alarm_io/__init__.h"
#include "shared-bindings/alarm_time/__init__.h"

extern int common_hal_alarm_get_sleep_time(void);
extern mp_obj_t common_hal_alarm_get_wake_alarm(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
