#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H

#include "py/obj.h"

extern mp_obj_t common_hal_sleepio_get_wake_alarm(void);
extern sleepio_reset_reason_t common_hal_sleepio_get_reset_reason(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H
