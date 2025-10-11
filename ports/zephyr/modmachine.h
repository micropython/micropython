#ifndef MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
#define MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H

#include "py/obj.h"
#include <zephyr/kernel.h>

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    const struct device *port;
    uint32_t pin;
    struct _machine_pin_irq_obj_t *irq;
} machine_pin_obj_t;

void machine_pin_deinit(void);

// External semaphore for lightsleep wake events
extern struct k_sem lightsleep_wake_sem;

#endif // MICROPY_INCLUDED_ZEPHYR_MODMACHINE_H
