#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H

#include "py/obj.h"

// This is implemented by shared-bindings so that implementations can set the
// newest alarm source.
extern void common_hal_sleepio_set_wake_alarm(mp_obj_t alarm);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_SLEEPIO___INIT___H
