#ifndef MICROPY_INCLUDED_RP2_MODMACHINE_H
#define MICROPY_INCLUDED_RP2_MODMACHINE_H

#include "py/obj.h"

void machine_pin_init(void);
void machine_pin_deinit(void);
void machine_i2s_init0(void);
void machine_i2s_deinit_all(void);
void machine_pwm_deinit_all(void);
void machine_uart_deinit_all(void);

struct _machine_spi_obj_t *spi_from_mp_obj(mp_obj_t o);

#endif // MICROPY_INCLUDED_RP2_MODMACHINE_H
