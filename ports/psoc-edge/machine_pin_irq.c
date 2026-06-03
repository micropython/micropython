/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "cy_gpio.h"
#include "sys_int.h"

#include "py/misc.h"
#include "py/runtime.h"
#include "shared/runtime/mpirq.h"
#include "machine_pin.h"

/* ======================================================================================================================== */


/* Forward declaration */
static void machine_pin_irq_port_handler(uint8_t port);
/**
 * All the pins of one port share the same IRQ handler.
 * Therefore, we can replicate the handlers based on the
 * available CPU GPIO ports that are exposed to the users.
 *
 * Alternatively, a single handler could have been used
 * for all ports and pins, but that would require sweeping
 * all ports and pins, which could be too much for an ISR.
 */
#define DEFINE_GPIO_PORT_IRQ_HANDLER(port) \
    void PORT##port##_IRQ_Handler(void) { \
        machine_pin_irq_port_handler(port); \
    }

/**
 * The file build-<board>/genhdr/pins.h contains the macro
 *
 *  MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(DO)
 *
 *  which uses the X-macro (as argument) pattern to pass a worker
 *  macro DO(port) for the list of all user available ports.
 *
 * The available (not hidden) user ports are those defined in the
 * boards/<board>/pins.csv file, which are not prefixed
 * with a hyphen(-).
 * See tools/boardgen.py and psoc-edge/boards/make-pins.py
 * for more information.
 */

/* Define all GPIO port IRQ handlers */
MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(DEFINE_GPIO_PORT_IRQ_HANDLER)

/**
 * Array of GPIO port IRQ configuration struct look up table.
 *
 * The IRQ number values are defined in cy_device_headers_ns.h
 * files for non-secure core.
 * The IRQn_Type value naming are different for the
 * secure core as they include "_sec_" in the names.
 * TODO: Review if secure core needs to be supported.
 * We can add conditional compilation using the macro CY_DEVICE_SECURE.
 */
static sys_int_cfg_t port_irq_cfg[MICROPY_PY_MACHINE_PIN_PORT_NUM_ENTRIES] = {
    #define MAP_GPIO_PORT_IRQ_CONFIG(port) [port] = { ioss_interrupts_gpio_##port##_IRQn, SYS_INT_IRQ_LOWEST_PRIORITY, PORT##port##_IRQ_Handler},
    MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(MAP_GPIO_PORT_IRQ_CONFIG)
};

/* ======================================================================================================================== */
/*  machine_pin_irq struct and API functions */

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
    sys_int_cfg_t *port_cfg;
} machine_pin_irq_obj_t;

machine_pin_irq_obj_t *machine_pin_irq_obj[MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES];

static machine_pin_irq_obj_t *machine_pin_get_irq_obj(machine_pin_obj_t *pin_obj) {
    uint32_t idx = 0;
    for (idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
        if (irq != NULL) {
            machine_pin_obj_t *irq_parent = MP_OBJ_TO_PTR(irq->base.parent);
            if (irq_parent == pin_obj) {
                return irq;
            }
        }
    }
    return NULL;
}

static machine_pin_irq_obj_t *machine_pin_irq_obj_allocate(void) {
    for (uint32_t idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
        if (irq == NULL) {
            irq = m_new_obj(machine_pin_irq_obj_t);
            machine_pin_irq_obj[idx] = irq;
            return irq;
        }
    }

    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("no more Pin IRQ objects available"));
}

static void machine_pin_irq_obj_dealloc(machine_pin_irq_obj_t *irq) {
    for (uint32_t idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        if (machine_pin_irq_obj[idx] == irq) {
            m_del_obj(machine_pin_irq_obj_t, irq);
            machine_pin_irq_obj[idx] = NULL;
            return;
        }
    }
}

static uint32_t machine_pin_irq_obj_find_index(uint8_t port, uint8_t pin) {
    uint32_t idx = 0;
    for (idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
        if (irq != NULL) {
            machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent);
            if (self->port == port && self->pin == pin) {
                return idx;
            }
        }
    }
    return idx;
}

static void machine_pin_irq_port_handler(uint8_t port) {
    /**
     * All pins in the same port will run the same handler.
     * Therefore we iterate over all the pins and check if their interrupt
     * status are active.
     */
    for (uint8_t pin = 0; pin < CY_GPIO_PINS_MAX; pin++) {
        if (Cy_GPIO_GetInterruptStatus(Cy_GPIO_PortToAddr(port), pin)) {
            /* Clear the GPIO source first so back-to-back edges can re-latch. */
            Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(port), pin);

            uint32_t idx = machine_pin_irq_obj_find_index(port, pin);
            if (idx >= MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES) {
                continue;
            }

            machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
            if (irq == NULL) {
                continue;
            }

            /**
             * Update the flags with the current trigger that caused the IRQ.
             * There is not a PDL function to get the current trigger,
             * so we use the configured trigger as the source of truth.
             * This means that if both edges are enabled, the flags will
             * indicate both edges even if only one of them caused the IRQ.
             */
            irq->flags = irq->trigger;
            if (irq->base.handler != mp_const_none) {
                mp_irq_handler(&irq->base);
            }
        }
    }
}

/**
 * Returns true if more than one pin irq objects are registered for the same port.
 * False otherwise.
 */
static bool machine_pin_irq_are_multiple_pin_irqs_registered_in_port(uint32_t port) {
    uint32_t pin_irq_registered_in_port = 0;
    for (uint32_t idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
        if (irq != NULL) {
            machine_pin_obj_t *pin_obj = MP_OBJ_TO_PTR(irq->base.parent);
            if (pin_obj->port == port) {
                if (++pin_irq_registered_in_port > 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Set the priority of the port interrupt.
 *
 * As all port pins share the same interrupt source, if multiple pins have an IRQ
 * registered, the desired new priority cannot be lower than the current configured priority.
 * This mechanism upgrades the priority of all pin port interrupts to ensure that higher priority
 * interrupts are not inadvertently masked by lower priority ones.
 */
static void machine_pin_irq_set_priority(sys_int_cfg_t *port_cfg, uint32_t port, uint32_t priority) {
    bool multiple_pin_irq_registered_in_port = machine_pin_irq_are_multiple_pin_irqs_registered_in_port(port);
    if (!multiple_pin_irq_registered_in_port) {
        port_cfg->priority = priority;
    } else {
        #define GPIO_PORT_HIGHER_PRIORITY(prio1, prio2) MIN(prio1, prio2) /* A smaller number means higher priority. */
        port_cfg->priority = GPIO_PORT_HIGHER_PRIORITY(port_cfg->priority, priority);
    }
}

#define MP_PIN_IRQ_TRIGGER_DISABLE  0x00UL
#define MP_PIN_IRQ_ALLOWED_FLAGS    (CY_GPIO_INTR_RISING | CY_GPIO_INTR_FALLING)

static void machine_pin_irq_config(machine_pin_irq_obj_t *irq, uint32_t trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent);
    if (trigger == MP_PIN_IRQ_TRIGGER_DISABLE) {
        sys_int_deinit(irq->port_cfg);
        Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(self->port), self->pin);
        Cy_GPIO_SetInterruptMask(Cy_GPIO_PortToAddr(self->port), self->pin, 0u);
        return;
    } else {
        Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(self->port), self->pin);
        Cy_GPIO_SetInterruptEdge(Cy_GPIO_PortToAddr(self->port), self->pin, trigger);
        Cy_GPIO_SetInterruptMask(Cy_GPIO_PortToAddr(self->port), self->pin, 1u);
        sys_int_init(irq->port_cfg);
    }
}

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = machine_pin_get_irq_obj(self);

    /* Check the trigger */
    mp_uint_t not_supported = trigger & ~MP_PIN_IRQ_ALLOWED_FLAGS;
    if (trigger != 0 && not_supported) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
    }

    irq->flags = 0;
    irq->trigger = trigger;

    machine_pin_irq_config(irq, trigger);

    return 0;
}

static mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = machine_pin_get_irq_obj(self);

    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

static const mp_irq_methods_t machine_pin_irq_methods = {
    .trigger = machine_pin_irq_trigger,
    .info = machine_pin_irq_info,
};

static void machine_pin_irq_obj_init(machine_pin_irq_obj_t *irq, machine_pin_obj_t *parent_pin) {
    mp_irq_init(&irq->base, &machine_pin_irq_methods, MP_OBJ_FROM_PTR(parent_pin));
    irq->flags = 0;
    irq->trigger = 0;
    irq->port_cfg = &port_irq_cfg[parent_pin->port];
}

static void machine_pin_irq_obj_deinit(machine_pin_irq_obj_t *irq) {
    irq->base.handler = mp_const_none;
    irq->base.ishard = false;
    irq->trigger = 0;
    irq->flags = 0;
    irq->port_cfg = NULL;
}

mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard, ARG_priority };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = CY_GPIO_INTR_RISING | CY_GPIO_INTR_FALLING} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_priority,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SYS_INT_IRQ_LOWEST_PRIORITY }},
    };

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_irq_obj_t *irq = machine_pin_get_irq_obj(self);
    if (irq == NULL) {
        irq = machine_pin_irq_obj_allocate();
        machine_pin_irq_obj_init(irq, self);
    }

    if (n_args > 1 || kw_args->used != 0) {
        /* Check the handler */
        mp_obj_t handler = args[ARG_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
            /* If handler is None, disable the IRQ */
        } else if (handler == mp_const_none) {
            machine_pin_irq_trigger(self, MP_PIN_IRQ_TRIGGER_DISABLE);
            return MP_OBJ_FROM_PTR(irq);
        }

        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        machine_pin_irq_set_priority(irq->port_cfg, self->port, args[ARG_priority].u_int);
        machine_pin_irq_trigger(self, args[ARG_trigger].u_int);
    }

    return MP_OBJ_FROM_PTR(irq);
}

void machine_pin_irq_deinit_all(void) {
    for (uint32_t i = 0; i < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; i++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[i];
        if (irq != NULL) {
            machine_pin_irq_config(irq, MP_PIN_IRQ_TRIGGER_DISABLE);
            machine_pin_irq_obj_deinit(irq);
            machine_pin_irq_obj_dealloc(irq);
        }
    }
}

// Register pin IRQ objects as roots so they are retained by the GC.
MP_REGISTER_ROOT_POINTER(struct _machine_pin_irq_obj_t *machine_pin_irq_obj[MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES]);
