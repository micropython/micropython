#ifndef MICROPY_INCLUDED_PSOC6_MODMACHINE_H
#define MICROPY_INCLUDED_PSOC6_MODMACHINE_H


// micropython includes
#include "py/obj.h"


extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_adcblock_type;
extern const mp_obj_type_t machine_i2s_type;

/* Note: the static functions' prototypes in the .c file cannot be declared here
since they are static. The static type in those functions come from MPY hence
should stay that way.  */


// functions to be called from other .c files. Hence they are not static.
void machine_init(void);
void machine_deinit(void);

#endif // MICROPY_INCLUDED_PSOC6_MODMACHINE_H
