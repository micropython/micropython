#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SLEEP___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SLEEP___INIT___H

#include "py/obj.h"

extern mp_obj_t common_hal_sleep_get_wake_alarm(void);
extern sleep_reset_reason_t common_hal_sleep_get_reset_reason(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPxs___INIT___H
