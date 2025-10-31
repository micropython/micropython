#ifndef MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
#define MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H

#include "py/obj.h"

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL

#include <pinctrl_soc.h>

typedef struct _zephyr_pinctrl_data_t {
    uint32_t pinmux;
    pinctrl_soc_pin_t pinctrl;
} zephyr_pinctrl_data_t;

#endif

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    const struct device *port;
    uint32_t pin;
    struct _machine_pin_irq_obj_t *irq;
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
    zephyr_pinctrl_data_t pinctrl_data;
    bool is_pinctrl;
    #endif
} machine_pin_obj_t;

void machine_pin_deinit(void);

#endif // MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
