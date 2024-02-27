/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include "machine_pin.h"

#include "hardware/irq.h"
#include "hardware/regs/intctrl.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"

// These can be or'd together.
#define GPIO_PULL_UP (1)
#define GPIO_PULL_DOWN (2)

#define GPIO_IRQ_ALL (0xf)

// Macros to access the state of the hardware.
#define GPIO_GET_FUNCSEL(id) ((iobank0_hw->io[(id)].ctrl & IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS) >> IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB)
#define GPIO_IS_OUT(id) (sio_hw->gpio_oe & (1 << (id)))
#define GPIO_IS_PULL_UP(id) (padsbank0_hw->io[(id)] & PADS_BANK0_GPIO0_PUE_BITS)
#define GPIO_IS_PULL_DOWN(id) (padsbank0_hw->io[(id)] & PADS_BANK0_GPIO0_PDE_BITS)

// Open drain behaviour is simulated.
#define GPIO_IS_OPEN_DRAIN(id) (machine_pin_open_drain_mask & (1 << (id)))

#ifndef MICROPY_HW_PIN_RESERVED
#define MICROPY_HW_PIN_RESERVED(i) (0)
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_af_type,
    MP_QSTR_PinAF,
    MP_TYPE_FLAG_NONE
    );

MP_DEFINE_CONST_OBJ_TYPE(
    pin_cpu_pins_obj_type,
    MP_QSTR_cpu,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_cpu_pins_locals_dict
    );

MP_DEFINE_CONST_OBJ_TYPE(
    pin_board_pins_obj_type,
    MP_QSTR_board,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_board_pins_locals_dict
    );

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} machine_pin_irq_obj_t;

static const mp_irq_methods_t machine_pin_irq_methods;
extern const machine_pin_obj_t machine_pin_obj_table[NUM_BANK0_GPIOS];

// Mask with "1" indicating that the corresponding pin is in simulated open-drain mode.
uint32_t machine_pin_open_drain_mask;

#if MICROPY_HW_PIN_EXT_COUNT
static inline bool is_ext_pin(__unused const machine_pin_obj_t *self) {
    return self->is_ext;
}
#else
#define is_ext_pin(x) false
#endif

static void gpio_irq(void) {
    for (int i = 0; i < 4; ++i) {
        uint32_t intr = iobank0_hw->intr[i];
        if (intr) {
            for (int j = 0; j < 8; ++j) {
                if (intr & 0xf) {
                    uint32_t gpio = 8 * i + j;
                    gpio_acknowledge_irq(gpio, intr & 0xf);
                    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[gpio]);
                    if (irq != NULL && (intr & irq->trigger)) {
                        irq->flags = intr & irq->trigger;
                        mp_irq_handler(&irq->base);
                    }
                }
                intr >>= 4;
            }
        }
    }
}

void machine_pin_init(void) {
    memset(MP_STATE_PORT(machine_pin_irq_obj), 0, sizeof(MP_STATE_PORT(machine_pin_irq_obj)));
    irq_add_shared_handler(IO_IRQ_BANK0, gpio_irq, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(IO_IRQ_BANK0, true);
    #if MICROPY_HW_PIN_EXT_COUNT
    machine_pin_ext_init();
    #endif
}

void machine_pin_deinit(void) {
    for (int i = 0; i < NUM_BANK0_GPIOS; ++i) {
        if (MICROPY_HW_PIN_RESERVED(i)) {
            continue;
        }
        gpio_set_irq_enabled(i, GPIO_IRQ_ALL, false);
    }
    irq_remove_handler(IO_IRQ_BANK0, gpio_irq);
}

const machine_pin_obj_t *machine_pin_find_named(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}

const machine_pin_af_obj_t *machine_pin_find_alt(const machine_pin_obj_t *pin, uint8_t fn) {
    const machine_pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->af_num; i++, af++) {
        if (af->fn == fn) {
            return af;
        }
    }
    return NULL;
}

const machine_pin_af_obj_t *machine_pin_find_alt_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx) {
    const machine_pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->af_num; i++, af++) {
        if (af->idx == af_idx) {
            return af;
        }
    }
    return NULL;
}

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin) {
    // Is already a object of the proper type
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin);
    }
    if (mp_obj_is_str(pin)) {
        const char *name = mp_obj_str_get_str(pin);
        // Try to find the pin in the board pins first.
        const machine_pin_obj_t *self = machine_pin_find_named(&machine_pin_board_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }
        // If not found, try to find the pin in the cpu pins which include
        // CPU and and externally controlled pins (if any).
        self = machine_pin_find_named(&machine_pin_cpu_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unknown named pin \"%s\""), name);
    } else if (mp_obj_is_int(pin)) {
        // get the wanted pin object
        int wanted_pin = mp_obj_get_int(pin);
        if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj_table)) {
            return &machine_pin_obj_table[wanted_pin];
        }
    }
    mp_raise_ValueError("invalid pin");
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    uint funcsel = GPIO_GET_FUNCSEL(self->id);
    qstr mode_qst;
    if (!is_ext_pin(self)) {
        if (funcsel == GPIO_FUNC_SIO) {
            if (GPIO_IS_OPEN_DRAIN(self->id)) {
                mode_qst = MP_QSTR_OPEN_DRAIN;
            } else if (GPIO_IS_OUT(self->id)) {
                mode_qst = MP_QSTR_OUT;
            } else {
                mode_qst = MP_QSTR_IN;
            }
        } else {
            mode_qst = MP_QSTR_ALT;
        }
        mp_printf(print, "Pin(%q, mode=%q", self->name, mode_qst);
        bool pull_up = false;
        if (GPIO_IS_PULL_UP(self->id)) {
            mp_printf(print, ", pull=%q", MP_QSTR_PULL_UP);
            pull_up = true;
        }
        if (GPIO_IS_PULL_DOWN(self->id)) {
            if (pull_up) {
                mp_printf(print, "|%q", MP_QSTR_PULL_DOWN);
            } else {
                mp_printf(print, ", pull=%q", MP_QSTR_PULL_DOWN);
            }
        }
        if (funcsel != GPIO_FUNC_SIO) {
            const machine_pin_af_obj_t *af = machine_pin_find_alt_by_index(self, funcsel);
            if (af == NULL) {
                mp_printf(print, ", alt=%u", funcsel);
            } else {
                mp_printf(print, ", alt=%q", af->name);
            }
        }
    } else {
        #if MICROPY_HW_PIN_EXT_COUNT
        mode_qst = (self->is_output) ? MP_QSTR_OUT : MP_QSTR_IN;
        mp_printf(print, "Pin(%q, mode=%q", self->name, mode_qst);
        #endif
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
    {MP_QSTR_alt,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_FUNC_SIO}},
};

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (is_ext_pin(self) && args[ARG_pull].u_obj != mp_const_none) {
        mp_raise_ValueError("pulls are not supported for external pins");
    }

    if (is_ext_pin(self) && args[ARG_alt].u_int != GPIO_FUNC_SIO) {
        mp_raise_ValueError("alternate functions are not supported for external pins");
    }

    // get initial value of pin (only valid for OUT and OPEN_DRAIN modes)
    int value = -1;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if (is_ext_pin(self)) {
            #if MICROPY_HW_PIN_EXT_COUNT
            // The regular Pins are const, but the external pins are mutable.
            machine_pin_obj_t *mutable_self = (machine_pin_obj_t *)self;
            machine_pin_ext_config(mutable_self, mode, value);
            #endif
        } else if (mode == MACHINE_PIN_MODE_IN) {
            mp_hal_pin_input(self->id);
        } else if (mode == MACHINE_PIN_MODE_OUT) {
            if (value != -1) {
                // set initial output value before configuring mode
                gpio_put(self->id, value);
            }
            mp_hal_pin_output(self->id);
        } else if (mode == MACHINE_PIN_MODE_OPEN_DRAIN) {
            mp_hal_pin_open_drain_with_value(self->id, value == -1 ? 1 : value);
        } else {
            // Configure alternate function.
            mp_uint_t af = args[ARG_alt].u_int;
            if (machine_pin_find_alt(self, af) == NULL) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin af: %d"), af);
            }
            gpio_set_function(self->id, af);
            machine_pin_open_drain_mask &= ~(1 << self->id);
        }
    }

    if (!is_ext_pin(self)) {
        // Configure pull (unconditionally because None means no-pull).
        uint32_t pull = 0;
        if (args[ARG_pull].u_obj != mp_const_none) {
            pull = mp_obj_get_int(args[ARG_pull].u_obj);
        }
        gpio_set_pulls(self->id, pull & GPIO_PULL_UP, pull & GPIO_PULL_DOWN);
    }
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
        if (is_ext_pin(self)) {
            #if MICROPY_HW_PIN_EXT_COUNT
            return MP_OBJ_NEW_SMALL_INT(machine_pin_ext_get(self));
            #endif
        } else {
            return MP_OBJ_NEW_SMALL_INT(gpio_get(self->id));
        }
    } else {
        // set pin
        bool value = mp_obj_is_true(args[0]);
        if (is_ext_pin(self)) {
            #if MICROPY_HW_PIN_EXT_COUNT
            machine_pin_ext_set(self, value);
            #endif
        } else if (GPIO_IS_OPEN_DRAIN(self->id)) {
            MP_STATIC_ASSERT(GPIO_IN == 0 && GPIO_OUT == 1);
            gpio_set_dir(self->id, 1 - value);
        } else {
            gpio_put(self->id, value);
        }
    }
    return mp_const_none;
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
    if (is_ext_pin(self)) {
        #if MICROPY_HW_PIN_EXT_COUNT
        machine_pin_ext_set(self, 0);
        #endif
    } else if (GPIO_IS_OPEN_DRAIN(self->id)) {
        gpio_set_dir(self->id, GPIO_OUT);
    } else {
        gpio_clr_mask(1u << self->id);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (is_ext_pin(self)) {
        #if MICROPY_HW_PIN_EXT_COUNT
        machine_pin_ext_set(self, 1);
        #endif
    } else if (GPIO_IS_OPEN_DRAIN(self->id)) {
        gpio_set_dir(self->id, GPIO_IN);
    } else {
        gpio_set_mask(1u << self->id);
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// pin.toggle()
static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (is_ext_pin(self)) {
        #if MICROPY_HW_PIN_EXT_COUNT
        machine_pin_ext_set(self, self->last_output_value ^ 1);
        #endif
    } else if (GPIO_IS_OPEN_DRAIN(self->id)) {
        if (GPIO_IS_OUT(self->id)) {
            gpio_set_dir(self->id, GPIO_IN);
        } else {
            gpio_set_dir(self->id, GPIO_OUT);
        }
    } else {
        gpio_xor_mask(1u << self->id);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static machine_pin_irq_obj_t *machine_pin_get_irq(mp_hal_pin_obj_t pin) {
    // Get the IRQ object.
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[pin]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_pin_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_pin_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(&machine_pin_obj_table[pin]);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(machine_pin_irq_obj[pin]) = irq;
    }
    return irq;
}

void mp_hal_pin_interrupt(mp_hal_pin_obj_t pin, mp_obj_t handler, mp_uint_t trigger, bool hard) {
    machine_pin_irq_obj_t *irq = machine_pin_get_irq(pin);

    // Disable all IRQs while data is updated.
    gpio_set_irq_enabled(pin, GPIO_IRQ_ALL, false);

    // Update IRQ data.
    irq->base.handler = handler;
    irq->base.ishard = hard;
    irq->flags = 0;
    irq->trigger = trigger;

    // Enable IRQ if a handler is given.
    if (handler != mp_const_none && trigger != MP_HAL_PIN_TRIGGER_NONE) {
        gpio_set_irq_enabled(pin, trigger, true);
    }
}

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = MP_HAL_PIN_TRIGGER_FALL | MP_HAL_PIN_TRIGGER_RISE} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (is_ext_pin(self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting a regular GPIO Pin"));
    }

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_irq_obj_t *irq = machine_pin_get_irq(self->id);

    if (n_args > 1 || kw_args->used != 0) {
        // Update IRQ data.
        mp_obj_t handler = args[ARG_handler].u_obj;
        mp_uint_t trigger = args[ARG_trigger].u_int;
        bool hard = args[ARG_hard].u_bool;
        mp_hal_pin_interrupt(self->id, handler, trigger, hard);
    }
    return MP_OBJ_FROM_PTR(irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

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
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MACHINE_PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MACHINE_PIN_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(MACHINE_PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_ALT), MP_ROM_INT(MACHINE_PIN_MODE_ALT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_IRQ_EDGE_RISE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_IRQ_EDGE_FALL) },

    // Pins alternate functions
    { MP_ROM_QSTR(MP_QSTR_ALT_SPI), MP_ROM_INT(GPIO_FUNC_SPI) },
    { MP_ROM_QSTR(MP_QSTR_ALT_UART), MP_ROM_INT(GPIO_FUNC_UART) },
    { MP_ROM_QSTR(MP_QSTR_ALT_I2C), MP_ROM_INT(GPIO_FUNC_I2C) },
    { MP_ROM_QSTR(MP_QSTR_ALT_PWM), MP_ROM_INT(GPIO_FUNC_PWM) },
    { MP_ROM_QSTR(MP_QSTR_ALT_SIO), MP_ROM_INT(GPIO_FUNC_SIO) },
    { MP_ROM_QSTR(MP_QSTR_ALT_PIO0), MP_ROM_INT(GPIO_FUNC_PIO0) },
    { MP_ROM_QSTR(MP_QSTR_ALT_PIO1), MP_ROM_INT(GPIO_FUNC_PIO1) },
    { MP_ROM_QSTR(MP_QSTR_ALT_GPCK), MP_ROM_INT(GPIO_FUNC_GPCK) },
    { MP_ROM_QSTR(MP_QSTR_ALT_USB), MP_ROM_INT(GPIO_FUNC_USB) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            if (is_ext_pin(self)) {
                #if MICROPY_HW_PIN_EXT_COUNT
                return machine_pin_ext_get(self);
                #endif
            } else {
                return gpio_get(self->id);
            }
        }
        case MP_PIN_WRITE: {
            if (is_ext_pin(self)) {
                #if MICROPY_HW_PIN_EXT_COUNT
                machine_pin_ext_set(self, arg);
                #endif
            } else {
                gpio_put(self->id, arg);
            }
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

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[self->id]);
    gpio_set_irq_enabled(self->id, GPIO_IRQ_ALL, false);
    irq->flags = 0;
    irq->trigger = new_trigger;
    gpio_set_irq_enabled(self->id, new_trigger, true);
    return 0;
}

static mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[self->id]);
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

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    const machine_pin_obj_t *pin = machine_pin_find(obj);
    if (is_ext_pin(pin)) {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting a regular GPIO Pin"));
    }
    return pin->id;
}

MP_REGISTER_ROOT_POINTER(void *machine_pin_irq_obj[30]);
