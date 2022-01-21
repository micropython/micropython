
#ifndef MICROPY_INCLUDED_RASPBERRRYPI_COMMON_HAL_COUNTIO_COUNTER_H
#define MICROPY_INCLUDED_RASPBERRRYPI_COMMON_HAL_COUNTIO_COUNTER_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin;
    uint8_t slice_num;
    mp_int_t count;
} countio_counter_obj_t;


void counter_interrupt_handler(void);

void reset_countio(void);

#endif // MICROPY_INCLUDED_RASPBERRRYPI_COMMON_HAL_COUNTIO_COUNTER_H
