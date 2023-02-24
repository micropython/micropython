#ifndef MICROPY_INCLUDED_RP2_MODMACHINE_H
#define MICROPY_INCLUDED_RP2_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2s_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_wdt_type;

void machine_pin_init(void);
void machine_pin_deinit(void);
void machine_i2s_init0(void);
void machine_pwm_deinit_all(void);

struct _machine_spi_obj_t *spi_from_mp_obj(mp_obj_t o);
NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_RP2_MODMACHINE_H
