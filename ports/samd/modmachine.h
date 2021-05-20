#ifndef MICROPY_INCLUDED_SAMD_MODMACHINE_H
#define MICROPY_INCLUDED_SAMD_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_led_type;
extern mp_obj_t machine_uart_init(void);
extern mp_obj_t machine_uart_deinit(void);

/*extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_hw_i2c_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_wdt_type;

void machine_pin_init(void);
void machine_pin_deinit(void);
*/
#endif // MICROPY_INCLUDED_SAMD_MODMACHINE_H
