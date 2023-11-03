#ifndef MICROPY_INCLUDED_RP2_MODMACHINE_H
#define MICROPY_INCLUDED_RP2_MODMACHINE_H

#include "py/obj.h"

void machine_pin_init(void);
void machine_pin_deinit(void);
void machine_i2s_init0(void);
void machine_pwm_deinit_all(void);

struct _machine_spi_obj_t *spi_from_mp_obj(mp_obj_t o);
NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_RP2_MODMACHINE_H
