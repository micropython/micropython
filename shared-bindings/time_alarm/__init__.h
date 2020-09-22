#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_TIME_ALARM___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_TIME_ALARM___INIT___H

#include "py/runtime.h"

typedef struct {
    mp_obj_base_t base;
} time_alarm_obj_t;

extern const mp_obj_type_t time_alarm_type;

extern void common_hal_time_alarm_duration(uint32_t);
extern void common_hal_time_alarm_disable (void);

#endif //MICROPY_INCLUDED_SHARED_BINDINGS_TIME_ALARM___INIT___H
