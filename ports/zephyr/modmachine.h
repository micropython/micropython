#ifndef MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
#define MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_hard_i2c_type;
extern const mp_obj_type_t machine_uart_type;

MP_DECLARE_CONST_FUN_OBJ_0(machine_info_obj);

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    const struct device *port;
    uint32_t pin;
    struct _machine_pin_irq_obj_t *irq;
} machine_pin_obj_t;

void machine_pin_deinit(void);

#endif // MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
