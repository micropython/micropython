#ifndef __ESPPWM_H__
#define __ESPPWM_H__

#include <stdbool.h>
#include <stdint.h>

void pwm_init(void);
void pwm_start(void);

void pwm_set_duty(uint16_t duty, uint8_t channel);
uint16_t pwm_get_duty(uint8_t channel);
void pwm_set_freq(uint16_t freq, uint8_t channel);
uint16_t pwm_get_freq(uint8_t channel);
int pwm_add(uint8_t pin_id, uint32_t pin_mux, uint32_t pin_func);
bool pwm_delete(uint8_t channel);

#endif
