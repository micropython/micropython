#ifndef MICROPY_INCLUDED_ESP32_MODMACHINE_H
#define MICROPY_INCLUDED_ESP32_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_touchpad_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_dac_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_hw_spi_type;
extern const mp_obj_type_t machine_uart_type;

void machine_pins_init(void);
void machine_pins_deinit(void);

#endif // MICROPY_INCLUDED_ESP32_MODMACHINE_H
