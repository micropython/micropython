#ifndef __MICROPY_INCLUDED_ESP8266_MODPYB_H__
#define __MICROPY_INCLUDED_ESP8266_MODPYB_H__

#include "py/obj.h"

extern const mp_obj_type_t pyb_pin_type;
extern const mp_obj_type_t pyb_pwm_type;
extern const mp_obj_type_t pyb_adc_type;
extern const mp_obj_type_t pyb_rtc_type;
extern const mp_obj_type_t pyb_uart_type;
extern const mp_obj_type_t pyb_i2c_type;
extern const mp_obj_type_t pyb_spi_type;

MP_DECLARE_CONST_FUN_OBJ(pyb_info_obj);

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

#endif // __MICROPY_INCLUDED_ESP8266_MODPYB_H__
