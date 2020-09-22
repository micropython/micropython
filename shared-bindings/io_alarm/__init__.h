#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_IO_ALARM___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_IO_ALARM___INIT___H

#include "py/runtime.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t gpio, level;
    bool pull;
} io_alarm_obj_t;

extern const mp_obj_type_t io_alarm_type;

extern mp_obj_t common_hal_io_alarm_pin_state (io_alarm_obj_t *self_in);
extern void common_hal_io_alarm_disable (void);

#endif //MICROPY_INCLUDED_SHARED_BINDINGS_IO_ALARM___INIT___H
