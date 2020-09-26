#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H

#include "py/obj.h"

extern void common_hal_alarm_disable_all(void);
extern mp_obj_t common_hal_alarm_get_wake_alarm(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
