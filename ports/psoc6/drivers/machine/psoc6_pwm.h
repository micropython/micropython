#ifndef MICROPY_INCLUDED_PSOC6_PWM_H
#define MICROPY_INCLUDED_PSOC6_PWM_H

// mpy includes
#include "py/runtime.h"

// MTB includes
#include "cyhal.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    cyhal_pwm_t pwm_obj;
    bool active;
    uint8_t pin;
    uint32_t fz;
    uint8_t duty_type;
    mp_float_t duty;
    bool invert;
} machine_pwm_obj_t;

cy_rslt_t pwm_freq_duty_set(cyhal_pwm_t *pwm_obj, uint32_t fz, float duty_cycle);
cy_rslt_t pwm_start(cyhal_pwm_t *pwm_obj);
cy_rslt_t pwm_init(machine_pwm_obj_t *machine_pwm_obj);
cy_rslt_t pwm_duty_set_ns(cyhal_pwm_t *pwm_obj, uint32_t fz, uint32_t pulse_width);
cy_rslt_t pwm_advanced_init(machine_pwm_obj_t *machine_pwm_obj);
void pwm_deinit(cyhal_pwm_t *pwm_obj);

#endif // MICROPY_INCLUDED_PSOC6_PWM_H
