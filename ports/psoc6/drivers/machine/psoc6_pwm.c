// MTB includes
#include "cyhal.h"

// port-specific includes
#include "psoc6_pwm.h"

cy_rslt_t pwm_freq_duty_set(cyhal_pwm_t *pwm_obj, uint32_t fz, float duty_cycle) {
    return cyhal_pwm_set_duty_cycle(pwm_obj, duty_cycle * 100, fz); // duty_cycle in percentage
}

cy_rslt_t pwm_start(cyhal_pwm_t *pwm_obj) {
    return cyhal_pwm_start(pwm_obj);
}

cy_rslt_t pwm_init(machine_pwm_obj_t *machine_pwm_obj) {
    return cyhal_pwm_init(&machine_pwm_obj->pwm_obj, machine_pwm_obj->pin, NULL);
}

void pwm_deinit(cyhal_pwm_t *pwm_obj) {
    cyhal_pwm_free(pwm_obj);
}

cy_rslt_t pwm_duty_set_ns(cyhal_pwm_t *pwm_obj, uint32_t fz, uint32_t pulse_width) {
    return cyhal_pwm_set_period(pwm_obj, 1000000 / fz, pulse_width * 1000);
}

cy_rslt_t pwm_advanced_init(machine_pwm_obj_t *machine_pwm_obj) {
    return cyhal_pwm_init_adv(&machine_pwm_obj->pwm_obj, machine_pwm_obj->pin, NC, CYHAL_PWM_LEFT_ALIGN, true, 0, true, NULL);
}
