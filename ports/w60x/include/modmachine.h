#ifndef MICROPY_INCLUDED_W600_MODMACHINE_H
#define MICROPY_INCLUDED_W600_MODMACHINE_H

#include "py/obj.h"

#define W600_YEAR_BASE 2000

extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_wdt_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_rtc_type;

#endif // MICROPY_INCLUDED_W600_MODMACHINE_H

