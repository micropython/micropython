#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_IO___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_IO___INIT___H

#include "py/runtime.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t gpio, level;
    bool pull;
} alarm_io_obj_t;

extern const mp_obj_type_t alarm_io_type;

extern mp_obj_t common_hal_alarm_io_pin_state (alarm_io_obj_t *self_in);
extern void common_hal_alarm_io_disable (void);

#endif //MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_IO___INIT___H
