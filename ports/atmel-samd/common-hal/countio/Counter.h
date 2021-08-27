
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNTER_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNTER_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin_a;
    uint8_t eic_channel_a : 4;
    mp_int_t count;
} countio_counter_obj_t;


void counter_interrupt_handler(uint8_t channel);

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNT_H
