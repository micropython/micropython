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
#include "cy_sysint.h"
#include "cy_device_headers.h"

#include "py/misc.h"
#include "shared/runtime/mpirq.h"
#include "machine_pin.h"

#define pin_irq_assert_raise_val(msg, ret)   if (ret != CY_SYSINT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/* ======================================================================================================================== */
/* Port IRQ structures and library API to support port-pin IRQ configuration */

/**
 * Define the lowest interrupt priority.
 * The cy_device_headers_ns.h file might
 * define the interrupt priority levels.
 */
#if defined(__NVIC_PRIO_BITS)
    #define GPIO_PORT_IRQ_LOWEST_PRIORITY ((1 << __NVIC_PRIO_BITS) - 1)
#else
    #define GPIO_PORT_IRQ_LOWEST_PRIORITY 7
#endif

typedef struct _port_irq_cfg_t {
    IRQn_Type irq_num;
    uint32_t priority;
    cy_israddress handler;
} port_irq_cfg_t;

/**
 * Initializes and enables the GPIO port IRQ.
 *
 * This function is meant to be called for each port pin IRQ enablement.
 * Therefore, the same port IRQ line can be reconfigured by calling this
 * function multiple times.
 * It enables as well the IRQ in the NVIC.
 */
static void port_irq_init(port_irq_cfg_t *port_cfg) {
    NVIC_ClearPendingIRQ(port_cfg->irq_num);
    NVIC_DisableIRQ(port_cfg->irq_num);

    cy_stc_sysint_t intr_cfg = {
        port_cfg->irq_num,
        port_cfg->priority
    };

    cy_en_sysint_status_t rslt = Cy_SysInt_Init(&intr_cfg, port_cfg->handler);
    pin_irq_assert_raise_val("Pin IRQ init failed (PSE PDL error code: %lx)", rslt);

    NVIC_EnableIRQ(port_cfg->irq_num);
}

/**
 * Deinit and disables the GPIO port IRQ.
 *
 * Calling deinit will disable the port interrupt for all registered IRQ pins
 * sharing the same port.
 * It can be called multiple times for the same port IRQ line,
 * without any adverse effects.
 */
static void port_irq_deinit(port_irq_cfg_t *port_cfg) {
    NVIC_ClearPendingIRQ(port_cfg->irq_num);
    NVIC_DisableIRQ(port_cfg->irq_num);
    port_cfg->priority = GPIO_PORT_IRQ_LOWEST_PRIORITY;
}

/* Forward declaration */
static bool machine_pin_irq_are_multiple_pin_irqs_registered_in_port(uint32_t port);
/**
 * Set the priority of the port interrupt.
 *
 * As all port pins share the same interrupt source, if multiple pins have an IRQ
 * registered, the desired new priority cannot be lower than the current configured priority.
 * This mechanism upgrades the priority of all pin port interrupts to ensure that higher priority
 * interrupts are not inadvertently masked by lower priority ones.
 */
static void port_irq_set_priority(port_irq_cfg_t *port_cfg, uint32_t port, uint32_t priority) {
    bool multiple_pin_irq_registered_in_port = machine_pin_irq_are_multiple_pin_irqs_registered_in_port(port);
    if (!multiple_pin_irq_registered_in_port) {
        port_cfg->priority = priority;
    } else {
        #define GPIO_PORT_HIGHER_PRIORITY(prio1, prio2) MIN(prio1, prio2) /* A smaller number means higher priority. */
        port_cfg->priority = GPIO_PORT_HIGHER_PRIORITY(port_cfg->priority, priority);
    }
}

static inline void port_irq_enable(port_irq_cfg_t *port_cfg) {
    NVIC_EnableIRQ(port_cfg->irq_num);
}

static inline void port_irq_disable(port_irq_cfg_t *port_cfg) {
    NVIC_DisableIRQ(port_cfg->irq_num);
}

static inline void port_irq_clear(port_irq_cfg_t *port_cfg) {
    NVIC_ClearPendingIRQ(port_cfg->irq_num);
}

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
static port_irq_cfg_t port_irq_cfg[MICROPY_PY_MACHINE_PIN_PORT_NUM_ENTRIES] = {
    #define MAP_GPIO_PORT_IRQ_CONFIG(port) [port] = { ioss_interrupts_gpio_##port##_IRQn, GPIO_PORT_IRQ_LOWEST_PRIORITY, PORT##port##_IRQ_Handler},
    MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(MAP_GPIO_PORT_IRQ_CONFIG)
};

/* ======================================================================================================================== */
/*  machine_pin_irq struct and API functions */

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
    port_irq_cfg_t *port_cfg;
} machine_pin_irq_obj_t;

machine_pin_irq_obj_t *machine_pin_irq_obj[MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES];

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

static uint32_t machine_pin_irq_obj_allocate_index(uint8_t port, uint8_t pin) {
    uint32_t idx = 0;
    for (idx = 0; idx < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; idx++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];
        if (irq == NULL) {
            return idx;
        }
    }

    return idx;
}

/**
 * Get the index of the Pin IRQ object if already registered
 * in the array. If not registered, allocate a new index.
 * If no more indexes are available, raise RuntimeError.
 */
static int machine_pin_irq_obj_get_index(uint8_t port, uint8_t pin) {
    int irq_index = machine_pin_irq_obj_find_index(port, pin);
    if (irq_index < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES) {
        return irq_index;
    }
    irq_index = machine_pin_irq_obj_allocate_index(port, pin);
    if (irq_index < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES) {
        return irq_index;
    }

    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("No more Pin IRQ objects available"));

    return -1;
}

static void machine_pin_irq_port_handler(uint8_t port) {
    /**
     * All pins in the same port will run the same handler.
     * Therefore we iterate over all the pins and check if their interrupt
     * status are active.
     */
    for (uint8_t pin = 0; pin < CY_GPIO_PINS_MAX; pin++) {
        if (Cy_GPIO_GetInterruptStatus(Cy_GPIO_PortToAddr(port), pin)) {
            uint32_t idx = machine_pin_irq_obj_find_index(port, pin);
            machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];

            /* TODO: This should be later handled by mp_irq_handler() which
            will schedule this call (all this requires the gc and the scheduler) */
            mp_call_function_0(irq->base.handler);

            Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(port), pin);
            port_irq_clear(irq->port_cfg);
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

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = machine_pin_irq_obj[machine_pin_irq_obj_find_index(self->port, self->pin)];

    irq->flags = 0; // TODO: What is this flags used for?
    irq->trigger = trigger; // TODO: Review this once mpirq module is enabled.

    Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(self->port), self->pin);
    Cy_GPIO_SetInterruptEdge(Cy_GPIO_PortToAddr(self->port), self->pin, trigger);
    Cy_GPIO_SetInterruptMask(Cy_GPIO_PortToAddr(self->port), self->pin, 1u);

    port_irq_init(irq->port_cfg);

    return 0;
}

// TODO: This will be useful when mpirq module is enabled.
// Review usage and implementation of triggers and flags.
static mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = machine_pin_irq_obj[machine_pin_irq_obj_find_index(self->port, self->pin)];

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

machine_pin_irq_obj_t *machine_pin_irq_get_irq(const machine_pin_obj_t *self) {
    int idx = machine_pin_irq_obj_get_index(self->port, self->pin);
    machine_pin_irq_obj_t *irq = machine_pin_irq_obj[idx];

    if (irq == NULL) {
        irq = m_new_obj(machine_pin_irq_obj_t);
        // TODO: This can be replaced by mp_irq_init() once gc is enabled
        // mp_irq_init(&irq->base, &machine_pin_irq_methods, MP_OBJ_FROM_PTR(self));
        // irq->base.base.type = &mp_irq_type; //-> Requires the garbage collector be enabled!
        irq->base.methods = (mp_irq_methods_t *)&machine_pin_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        irq->port_cfg = &port_irq_cfg[self->port];
        machine_pin_irq_obj[idx] = irq;
    }

    return irq;
}

mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard, ARG_priority };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = CY_GPIO_INTR_RISING | CY_GPIO_INTR_FALLING} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_priority,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_PORT_IRQ_LOWEST_PRIORITY }},
    };

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_irq_obj_t *irq = machine_pin_irq_get_irq(self);

    if (n_args > 1 || kw_args->used != 0) {
        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        port_irq_set_priority(irq->port_cfg, self->port, args[ARG_priority].u_int);
        machine_pin_irq_trigger(self, args[ARG_trigger].u_int);
    }

    return mp_const_none;
    // return MP_OBJ_FROM_PTR(irq); // TODO: Do we need to return the irq object? We could return irq->base. once mpirq mode is enabled.
}

void machine_pin_irq_deinit_all(void) {
    for (uint32_t i = 0; i < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; i++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[i];
        if (irq != NULL) {
            port_irq_deinit(irq->port_cfg);

            machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent);
            Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(self->port), self->pin);
            Cy_GPIO_SetInterruptMask(Cy_GPIO_PortToAddr(self->port), self->pin, 0u);

            m_del_obj(machine_pin_irq_obj_t, irq);
            machine_pin_irq_obj[i] = NULL;
        }
    }
}

// TODO: This will be enabled once the mpirq is fully functional
// and when the gc is enabled.
// The array of pin_irq object will be part of the root pointers and under garbage collection
// MP_REGISTER_ROOT_POINTER(void *machine_pin_irq_obj[MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES]);
