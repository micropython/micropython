#ifndef MICROPY_INCLUDED_ESP32_MODMACHINE_H
#define MICROPY_INCLUDED_ESP32_MODMACHINE_H

#include "py/obj.h"

typedef enum {
    // MACHINE_WAKE_IDLE=0x01,
    MACHINE_WAKE_SLEEP=0x02,
    MACHINE_WAKE_DEEPSLEEP=0x04
} wake_type_t;

extern const mp_obj_type_t machine_touchpad_type;
extern const mp_obj_type_t machine_dac_type;
extern const mp_obj_type_t machine_sdcard_type;

void machine_init(void);
void machine_deinit(void);
void machine_pins_init(void);
void machine_pins_deinit(void);
void machine_pwm_deinit_all(void);
// TODO: void machine_rmt_deinit_all(void);
void machine_timer_deinit_all(void);
void machine_uart_deinit_all(void);
void machine_i2s_init0();

#endif // MICROPY_INCLUDED_ESP32_MODMACHINE_H
