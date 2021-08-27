#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_COUNTIO_COUNTER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_COUNTIO_COUNTER_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/countio/Counter.h"

extern const mp_obj_type_t countio_counter_type;

extern void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin_a);
extern void common_hal_countio_counter_deinit(countio_counter_obj_t *self);
extern bool common_hal_countio_counter_deinited(countio_counter_obj_t *self);
extern mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self);
extern void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count);
extern void common_hal_countio_counter_reset(countio_counter_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_COUNTIO_COUNTER_H
