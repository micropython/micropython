#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TOUCH___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TOUCH___INIT___H

#include "py/runtime.h"

typedef struct {
    mp_obj_base_t base;
} alarm_touch_obj_t;

extern const mp_obj_type_t alarm_touch_type;

extern void common_hal_alarm_touch_disable (void);

#endif //MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TOUCH___INIT___H
