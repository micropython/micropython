#ifndef MICROPY_INCLUDED_ESP8266_MODMACHINE_H
#define MICROPY_INCLUDED_ESP8266_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t pyb_pin_type;
extern const mp_obj_type_t pyb_pwm_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t pyb_rtc_type;
extern const mp_obj_type_t pyb_uart_type;
extern const mp_obj_type_t pyb_i2c_type;
extern const mp_obj_type_t machine_hspi_type;

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_info_obj);

typedef struct _pyb_pin_obj_t {
    mp_obj_base_t base;
    uint16_t phys_port;
    uint16_t func;
    uint32_t periph;
} pyb_pin_obj_t;

const pyb_pin_obj_t pyb_pin_obj[16 + 1];

void pin_init0(void);
void pin_intr_handler_iram(void *arg);
void pin_intr_handler(uint32_t);

uint mp_obj_get_pin(mp_obj_t pin_in);
pyb_pin_obj_t *mp_obj_get_pin_obj(mp_obj_t pin_in);
int pin_get(uint pin);
void pin_set(uint pin, int value);

extern uint32_t pyb_rtc_alarm0_wake;
extern uint64_t pyb_rtc_alarm0_expiry;

void pyb_rtc_set_us_since_2000(uint64_t nowus);
uint64_t pyb_rtc_get_us_since_2000();
void rtc_prepare_deepsleep(uint64_t sleep_us);

#endif // MICROPY_INCLUDED_ESP8266_MODMACHINE_H
