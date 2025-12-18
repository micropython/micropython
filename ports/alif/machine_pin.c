/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "shared/runtime/mpirq.h"

#ifndef MACHINE_PIN_NUM_VECTORS
#define MACHINE_PIN_NUM_PORT_IO (8)
#define MACHINE_PIN_NUM_VECTORS (16 * MACHINE_PIN_NUM_PORT_IO)
#endif

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
    IRQn_Type irq_num;
    bool reserved; // for use by other drivers
} machine_pin_irq_obj_t;

#define MACHINE_PIN_IRQ_INDEX(port, pin) \
    ((port) * MACHINE_PIN_NUM_PORT_IO + (pin))

#define MACHINE_PIN_IRQ_OBJECT(port, pin) \
    (MP_STATE_PORT(machine_pin_irq_obj[MACHINE_PIN_IRQ_INDEX((port), (pin))]))

// Defines a single GPIO IRQ handler
#define DEFINE_GPIO_IRQ_HANDLER(pname, port, pin) \
    void pname##_IRQ##pin##Handler(void) { \
        machine_pin_irq_obj_t *irq = MACHINE_PIN_IRQ_OBJECT(port, pin); \
        machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent); \
        gpio_interrupt_eoi(self->gpio, pin); \
        irq->flags = irq->trigger; \
        mp_irq_handler(&irq->base); \
    }

// Defines all 8 pin IRQ handlers for a port
#define DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(gpio, port) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 0) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 1) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 2) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 3) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 4) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 5) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 6) \
    DEFINE_GPIO_IRQ_HANDLER(gpio, port, 7)

// Generate handlers for GPIO ports 0 to 14 + LPGPIO
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO0, 0)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO1, 1)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO2, 2)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO3, 3)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO4, 4)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO5, 5)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO6, 6)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO7, 7)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO8, 8)

DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 0)
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 1)
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 2)
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 3)
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 4)
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 5)
// DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 6)  // Reserved for WiFi
DEFINE_GPIO_IRQ_HANDLER(GPIO9, 9, 7)

DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO10, 10)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO11, 11)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO12, 12)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO13, 13)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(GPIO14, 14)
DEFINE_GPIO_IRQ_HANDLERS_FOR_PORT(LPGPIO, 15)

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;
static const mp_irq_methods_t machine_pin_irq_methods;

static const machine_pin_obj_t *machine_pin_find_named(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin) {
    // Is already a object of the proper type
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin);
    }
    if (mp_obj_is_str(pin)) {
        // Try to find the pin in the board pins first.
        const machine_pin_obj_t *self = machine_pin_find_named(&machine_pin_board_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }

        // If not found, try to find the pin in the cpu pins.
        self = machine_pin_find_named(&machine_pin_cpu_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }

        // Pin name not found.
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unknown named pin \"%s\""), mp_obj_str_get_str(pin));
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;

    uint8_t alt_func, pad_ctrl;
    pinconf_get(self->port, self->pin, &alt_func, &pad_ctrl);

    qstr mode_qst;
    if (gpio_get_direction(self->gpio, self->pin) == GPIO_PIN_DIR_INPUT) {
        mode_qst = MP_QSTR_IN;
    } else {
        if (pad_ctrl & PADCTRL_DRIVER_OPEN_DRAIN) {
            mode_qst = MP_QSTR_OPEN_DRAIN;
        } else {
            mode_qst = MP_QSTR_OUT;
        }
    }
    mp_printf(print, "Pin(%q, mode=%q", self->name, mode_qst);
    uint8_t pad_ctrl_pull = pad_ctrl & (PADCTRL_DRIVER_DISABLED_PULL_DOWN | PADCTRL_DRIVER_DISABLED_PULL_UP);
    if (pad_ctrl_pull == PADCTRL_DRIVER_DISABLED_PULL_UP) {
        mp_printf(print, ", pull=%q", MP_QSTR_PULL_UP);
    } else if (pad_ctrl_pull == PADCTRL_DRIVER_DISABLED_PULL_DOWN) {
        mp_printf(print, ", pull=%q", MP_QSTR_PULL_DOWN);
    }
    if (alt_func != PINMUX_ALTERNATE_FUNCTION_0) {
        mp_printf(print, ", alt=%u", alt_func);
    }
    mp_printf(print, ")");
}

enum {
    ARG_mode, ARG_pull, ARG_value, ARG_alt
};
static const mp_arg_t allowed_args[] = {
    {MP_QSTR_mode,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_pull,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_alt,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
};

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get initial value of pin (only valid for OUT and OPEN_DRAIN modes)
    int value = -1;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if (mode == MP_HAL_PIN_MODE_INPUT) {
            mp_hal_pin_input(self);
        } else if (mode == MP_HAL_PIN_MODE_OUTPUT) {
            if (value != -1) {
                // set initial output value before configuring mode
                mp_hal_pin_write(self, value);
            }
            mp_hal_pin_output(self);
        } else if (mode == MP_HAL_PIN_MODE_OPEN_DRAIN) {
            if (value != -1) {
                // set initial output value before configuring mode
                mp_hal_pin_write(self, value);
            }
            mp_hal_pin_open_drain(self);
        }
    }

    // Configure pull (unconditionally because None means no-pull).
    uint32_t pull = 0;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }
    uint8_t alt_func;
    uint8_t pad_ctrl;
    pinconf_get(self->port, self->pin, &alt_func, &pad_ctrl);
    alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    pad_ctrl |= PADCTRL_READ_ENABLE;
    pad_ctrl &= ~(PADCTRL_DRIVER_DISABLED_PULL_DOWN | PADCTRL_DRIVER_DISABLED_PULL_UP);
    if (pull & MP_HAL_PIN_PULL_UP) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_UP;
    }
    if (pull & MP_HAL_PIN_PULL_DOWN) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_DOWN;
    }
    pinconf_set(self->port, self->pin, alt_func, pad_ctrl);

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *self = machine_pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        bool value = mp_obj_is_true(args[0]);
        mp_hal_pin_write(self, value);
        return mp_const_none;
    }
}

// pin.init(mode, pull)
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.low()
static mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_low(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_high(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// pin.toggle()
static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_toggle_value(self->gpio, self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MACHINE_PIN_IRQ_OBJECT(self->port, self->pin);

    irq->flags = 0;
    irq->trigger = trigger;

    // Disable IRQs.
    gpio_disable_interrupt(self->gpio, self->pin);
    gpio_mask_interrupt(self->gpio, self->pin);

    NVIC_ClearPendingIRQ(irq->irq_num);
    NVIC_DisableIRQ(irq->irq_num);

    // Return if the trigger is disabled.
    if (trigger == 0) {
        return 0;
    }

    // Clear and enable GPIO IRQ.
    gpio_enable_interrupt(self->gpio, self->pin);
    gpio_unmask_interrupt(self->gpio, self->pin);

    // Clear GPIO config.
    self->gpio->GPIO_INT_BOTHEDGE &= ~(1 << self->pin);
    self->gpio->GPIO_INT_POLARITY &= ~(1 << self->pin);
    self->gpio->GPIO_INTTYPE_LEVEL &= ~(1 << self->pin);

    // Configure GPIO IRQ trigger
    if (trigger == MP_HAL_PIN_TRIGGER_FALL) {
        gpio_interrupt_set_edge_trigger(self->gpio, self->pin);
        gpio_interrupt_set_polarity_low(self->gpio, self->pin);
    } else if (trigger == MP_HAL_PIN_TRIGGER_RISE) {
        gpio_interrupt_set_edge_trigger(self->gpio, self->pin);
        gpio_interrupt_set_polarity_high(self->gpio, self->pin);
    } else if (trigger == (MP_HAL_PIN_TRIGGER_FALL | MP_HAL_PIN_TRIGGER_RISE)) {
        gpio_interrupt_set_both_edge_trigger(self->gpio, self->pin);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid IRQ trigger"));
    }

    // Clear GPIO IRQ (must be done after configuring trigger)
    gpio_interrupt_eoi(self->gpio, self->pin);

    // Clear and enable NVIC GPIO IRQ.
    NVIC_ClearPendingIRQ(irq->irq_num);
    NVIC_SetPriority(irq->irq_num, IRQ_PRI_GPIO);
    NVIC_EnableIRQ(irq->irq_num);

    return 0;
}

static mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MACHINE_PIN_IRQ_OBJECT(self->port, self->pin);

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

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = MP_HAL_PIN_TRIGGER_FALL | MP_HAL_PIN_TRIGGER_RISE} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_irq_obj_t *irq = MACHINE_PIN_IRQ_OBJECT(self->port, self->pin);

    // Allocate a new IRQ object if it doesn't exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_pin_irq_obj_t);
        uint32_t idx = MACHINE_PIN_IRQ_INDEX(self->port, self->pin);

        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_pin_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        irq->reserved = false;
        irq->irq_num = (self->port < 15) ? (GPIO0_IRQ0_IRQn + idx) : (LPGPIO_IRQ0_IRQn + self->pin);
        MP_STATE_PORT(machine_pin_irq_obj[idx]) = irq;
    }

    if (n_args > 1 || kw_args->used != 0) {
        if (irq->reserved) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Pin IRQ is reserved"));
        }
        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        machine_pin_irq_trigger(self, args[ARG_trigger].u_int);
    }

    return MP_OBJ_FROM_PTR(irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

void machine_pin_irq_deinit(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(machine_pin_irq_obj)); i++) {
        machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[i]);
        if (irq != NULL) {
            machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent);
            machine_pin_irq_trigger(self, 0);
            MP_STATE_PORT(machine_pin_irq_obj[i]) = NULL;
        }
    }
}

static MP_DEFINE_CONST_OBJ_TYPE(
    pin_cpu_pins_obj_type,
    MP_QSTR_cpu,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_cpu_pins_locals_dict
    );

static MP_DEFINE_CONST_OBJ_TYPE(
    pin_board_pins_obj_type,
    MP_QSTR_board,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_board_pins_locals_dict
    );

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu), MP_ROM_PTR(&pin_cpu_pins_obj_type) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MP_HAL_PIN_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MP_HAL_PIN_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(MP_HAL_PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(MP_HAL_PIN_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(MP_HAL_PIN_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(MP_HAL_PIN_TRIGGER_FALL) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(MP_HAL_PIN_TRIGGER_RISE) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            return mp_hal_pin_read(self);
        }
        case MP_PIN_WRITE: {
            mp_hal_pin_write(self, arg);
            return 0;
        }
    }
    return -1;
}

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    return machine_pin_find(obj);
}

MP_REGISTER_ROOT_POINTER(void *machine_pin_irq_obj[MACHINE_PIN_NUM_VECTORS]);
