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

#include "shared/runtime/mpirq.h"
#include "machine_pin.h"

#define pin_irq_assert_raise_val(msg, ret)   if (ret != CY_SYSINT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
    IRQn_Type irq_num;
    uint32_t priority;
} machine_pin_irq_obj_t;

machine_pin_irq_obj_t *machine_pin_irq_obj[MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES];

/*
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

/* Forward declaration of all ports IRQ handler*/
void port_any_irq_handler(uint8_t port);

/**
 * All the pins of one port share the same IRQ handler.
 * Therefore, we can replicate the handlers based on the
 * available CPU GPIO ports that are exposed to the users.
 * Alternatively, a single handler can be used for all ports
 * and pins, but that would require sweeping all ports and
 * pins, which could be inefficient, specially for a ISR.
 */
#define DEFINE_GPIO_PORT_IRQ_HANDLER(port) \
    void PORT##port##_IRQ_Handler(void) { \
        port_any_irq_handler(port); \
    }

/* Define all GPIO port IRQ handlers */
MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(DEFINE_GPIO_PORT_IRQ_HANDLER)

/**
 * Array of GPIO port IRQ numbers look up table.
 *
 * This table map values are defined in cy_device_headers_ns.h
 * files for non-secure core.
 * The IRQn_Type value naming are different for the
 * secure core as they include "_sec_" in the names.
 * TODO: Review if secure core needs to be supported.
 * We can add conditional compilation using the macro CY_DEVICE_SECURE.
 */
const static IRQn_Type gpio_port_irq_num[MICROPY_PY_MACHINE_PIN_PORT_NUM_ENTRIES] = {
    #define MAP_GPIO_PORT_IRQ_NUM(port) [port] = ioss_interrupts_gpio_##port##_IRQn,
    MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(MAP_GPIO_PORT_IRQ_NUM)
};

/* Array of GPIO port IRQ handlers look up table */
const static void (*gpio_port_irq_handlers[MICROPY_PY_MACHINE_PIN_PORT_NUM_ENTRIES])(void) = {
    #define MAP_GPIO_PORT_IRQ_HANDLER(port) [port] = PORT##port##_IRQ_Handler,
    MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(MAP_GPIO_PORT_IRQ_HANDLER)
};

uint32_t machine_pin_irq_obj_find_index(uint8_t port, uint8_t pin) {
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

uint32_t machine_pin_irq_obj_allocate_index(uint8_t port, uint8_t pin) {
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
int machine_pin_irq_obj_get_index(uint8_t port, uint8_t pin) {
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

void port_any_irq_handler(uint8_t port) {
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
            NVIC_ClearPendingIRQ(gpio_port_irq_num[port]);
        }
    }
}

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = machine_pin_irq_obj[machine_pin_irq_obj_find_index(self->port, self->pin)];

    irq->flags = 0; // TODO: What is this flags used for?
    irq->trigger = trigger; // TODO: Review this once mpirq module is enabled.

    Cy_GPIO_ClearInterrupt(Cy_GPIO_PortToAddr(self->port), self->pin);
    IRQn_Type irq_num = gpio_port_irq_num[self->port];

    NVIC_ClearPendingIRQ(irq_num);
    NVIC_DisableIRQ(irq_num);

    cy_stc_sysint_t intrCfg =
    {
        irq_num,       /* Interrupt source */
        irq->priority  /* Interrupt priority -> TODO: what if 2 pins with the same IRQ set different priorities?? */
    };

    cy_en_sysint_status_t rslt = Cy_SysInt_Init(&intrCfg, gpio_port_irq_handlers[self->port]);
    pin_irq_assert_raise_val("Pin IRQ init failed (PSE PDL error code: %lx)", rslt);

    Cy_GPIO_SetInterruptEdge(Cy_GPIO_PortToAddr(self->port), self->pin, trigger);
    Cy_GPIO_SetInterruptMask(Cy_GPIO_PortToAddr(self->port), self->pin, 1u);

    NVIC_EnableIRQ(intrCfg.intrSrc);

    return 0;
}

// This will be useful when mpirq module is enabled.
// TODO: Review usage and implementation of triggers and flags.
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
        irq->irq_num = gpio_port_irq_num[self->port];
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
        { MP_QSTR_priority,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7 }},
    };

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_irq_obj_t *irq = machine_pin_irq_get_irq(self);

    if (n_args > 1 || kw_args->used != 0) {

        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        irq->priority = args[ARG_priority].u_int;

        machine_pin_irq_trigger(self, args[ARG_trigger].u_int);
    }

    return mp_const_none;
    // return MP_OBJ_FROM_PTR(irq); // TODO: Do we need to return the irq object? We could return irq->base. once mpirq mode is enabled.
}

void machine_pin_irq_deinit_all(void) {
    for (uint32_t i = 0; i < MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES; i++) {
        machine_pin_irq_obj_t *irq = machine_pin_irq_obj[i];
        if (irq != NULL) {
            /* If there are several pins connected to the same IRQ
            the NVIC will be disabled already when the first pin is deinitialized.
            This implementation assumes subsequent calls after disabling the IRQ
            have no effect and are handled gracefully.
            And simplifies the implementation, as we don't need to keep track of
            the state of each port separately. */
            NVIC_ClearPendingIRQ(irq->irq_num);
            NVIC_DisableIRQ(irq->irq_num);

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
