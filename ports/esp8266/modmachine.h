#ifndef MICROPY_INCLUDED_ESP8266_MODMACHINE_H
#define MICROPY_INCLUDED_ESP8266_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t pyb_pin_type;
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

#define GPIO_MODE_INPUT (0)
#define GPIO_MODE_OUTPUT (1)
#define GPIO_MODE_OPEN_DRAIN (2) // synthesised
#define GPIO_PULL_NONE (0)
#define GPIO_PULL_UP (1)
// Removed in SDK 1.1.0
// #define GPIO_PULL_DOWN (2)

extern uint8_t pin_mode[16 + 1];

void pin_init0(void);

uint mp_obj_get_pin(mp_obj_t pin_in);
pyb_pin_obj_t *mp_obj_get_pin_obj(mp_obj_t pin_in);
int pin_get(uint pin);
void pin_set(uint pin, int value);

extern uint32_t pyb_rtc_alarm0_wake;
extern uint64_t pyb_rtc_alarm0_expiry;

void pyb_rtc_set_us_since_epoch(uint64_t nowus);
uint64_t pyb_rtc_get_us_since_epoch();
void rtc_prepare_deepsleep(uint64_t sleep_us);

#endif // MICROPY_INCLUDED_ESP8266_MODMACHINE_H
