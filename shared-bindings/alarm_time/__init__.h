#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TIME___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TIME___INIT___H

#include "py/runtime.h"

typedef struct {
    mp_obj_base_t base;
} alarm_time_obj_t;

extern const mp_obj_type_t alarm_time_type;

extern void common_hal_alarm_time_duration (uint32_t);
extern void common_hal_alarm_time_disable (void);

#endif //MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_TIME___INIT___H
