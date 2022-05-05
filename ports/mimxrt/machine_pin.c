/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
 * Copyright (c) 2021 Robert Hammelrath
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
#include <stdint.h>

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/mpirq.h"
#include "extmod/virtpin.h"
#include "pin.h"

// Local functions
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

// Class Methods
STATIC void machine_pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind);
STATIC mp_obj_t machine_pin_obj_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

// Instance Methods
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in);
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in);
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args);
STATIC mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

// Local data
enum {
    PIN_INIT_ARG_MODE = 0,
    PIN_INIT_ARG_PULL,
    PIN_INIT_ARG_VALUE,
    PIN_INIT_ARG_DRIVE,
};

// Pin mapping dictionaries
const mp_obj_type_t machine_pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .locals_dict = (mp_obj_t)&machine_pin_cpu_pins_locals_dict,
};

const mp_obj_type_t machine_pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .locals_dict = (mp_obj_t)&machine_pin_board_pins_locals_dict,
};

STATIC const mp_irq_methods_t machine_pin_irq_methods;

static GPIO_Type *gpiobases[] = GPIO_BASE_PTRS;
STATIC const uint16_t GPIO_combined_low_irqs[] = GPIO_COMBINED_LOW_IRQS;
STATIC const uint16_t GPIO_combined_high_irqs[] = GPIO_COMBINED_HIGH_IRQS;
STATIC const uint16_t IRQ_mapping[] = {kGPIO_NoIntmode, kGPIO_IntRisingEdge, kGPIO_IntFallingEdge, kGPIO_IntRisingOrFallingEdge};
#define GET_PIN_IRQ_INDEX(gpio_nr, pin) ((gpio_nr - 1) * 32 + pin)

int GPIO_get_instance(GPIO_Type *gpio) {
    int gpio_nr;
    for (gpio_nr = 0; gpio_nr < ARRAY_SIZE(gpiobases); gpio_nr++) {
        if (gpio == gpiobases[gpio_nr]) {
            return gpio_nr;
        }
    }
    return 0;
}

void call_handler(GPIO_Type *gpio, int gpio_nr, int pin) {
    uint32_t mask = 1 << pin;
    uint32_t isr = gpio->ISR & gpio->IMR;
    for (int i = 0; i < 16; i++, pin++, mask <<= 1) {
        // Did the ISR fire? Consider only the bits that are enabled.
        if (isr & mask) {
            gpio->ISR = mask; // clear the ISR flag
            int index = GET_PIN_IRQ_INDEX(gpio_nr, pin);
            machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_objects[index]);
            if (irq != NULL) {
                irq->flags = irq->trigger;
                mp_irq_handler(&irq->base);
            }
        }
    }
}

// 10 GPIO IRQ handlers, each covering 16 bits.

void GPIO1_Combined_0_15_IRQHandler(void) {
    call_handler(gpiobases[1], 1, 0);
}

void GPIO1_Combined_16_31_IRQHandler(void) {
    call_handler(gpiobases[1], 1, 16);
}

void GPIO2_Combined_0_15_IRQHandler(void) {
    call_handler(gpiobases[2], 2, 0);
}

void GPIO2_Combined_16_31_IRQHandler(void) {
    call_handler(gpiobases[2], 2, 16);
}

void GPIO3_Combined_0_15_IRQHandler(void) {
    call_handler(gpiobases[3], 3, 0);
}

void GPIO3_Combined_16_31_IRQHandler(void) {
    call_handler(gpiobases[3], 3, 16);
}

void GPIO4_Combined_0_15_IRQHandler(void) {
    call_handler(gpiobases[4], 4, 0);
}

void GPIO4_Combined_16_31_IRQHandler(void) {
    call_handler(gpiobases[4], 4, 16);
}

void GPIO5_Combined_0_15_IRQHandler(void) {
    call_handler(gpiobases[5], 5, 0);
}

void GPIO5_Combined_16_31_IRQHandler(void) {
    call_handler(gpiobases[5], 5, 16);
}

// Deinit all pin IRQ handlers.
void machine_pin_irq_deinit(void) {
    for (int i = 0; i < ARRAY_SIZE(MP_STATE_PORT(machine_pin_irq_objects)); ++i) {
        machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_objects[i]);
        if (irq != NULL) {
            machine_pin_obj_t *self = MP_OBJ_TO_PTR(irq->base.parent);
            GPIO_PortDisableInterrupts(self->gpio, 1U << self->pin);
            MP_STATE_PORT(machine_pin_irq_objects[i]) = NULL;
        }
    }
}

// Simplified mode setting used by the extmod modules
void machine_pin_set_mode(const machine_pin_obj_t *self, uint8_t mode) {
    gpio_pin_config_t pin_config = {kGPIO_DigitalInput, 1, kGPIO_NoIntmode};

    pin_config.direction = (mode == PIN_MODE_IN ? kGPIO_DigitalInput : kGPIO_DigitalOutput);
    GPIO_PinInit(self->gpio, self->pin, &pin_config);
    if (mode == PIN_MODE_OPEN_DRAIN) {
        uint32_t pad_config = *(uint32_t *)self->configRegister;
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b1) | IOMUXC_SW_PAD_CTL_PAD_DSE(0b110);
        IOMUXC_SetPinMux(self->muxRegister, PIN_AF_MODE_ALT5, 0, 0, self->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(self->muxRegister, PIN_AF_MODE_ALT5, 0, 0, self->configRegister, pad_config);
    }
}

STATIC mp_obj_t machine_pin_obj_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;

    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        [PIN_INIT_ARG_MODE] { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        [PIN_INIT_ARG_PULL] { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        [PIN_INIT_ARG_VALUE] { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        [PIN_INIT_ARG_DRIVE] { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PIN_DRIVE_3}},
        // TODO: Implement additional arguments
        /*
        { MP_QSTR_af, MP_ARG_INT, {.u_int = -1}}, // legacy
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},*/
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get io mode
    uint mode = args[PIN_INIT_ARG_MODE].u_int;
    if (!IS_GPIO_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
    }

    // Handle configuration for GPIO
    if ((mode == PIN_MODE_IN) || (mode == PIN_MODE_OUT) || (mode == PIN_MODE_OPEN_DRAIN)) {
        gpio_pin_config_t pin_config;
        const machine_pin_af_obj_t *af_obj;
        uint32_t pad_config = 0UL;
        uint8_t pull = PIN_PULL_DISABLED;
        uint32_t drive = (uint32_t)args[PIN_INIT_ARG_DRIVE].u_int;

        // Generate pin configuration
        if ((args[PIN_INIT_ARG_VALUE].u_obj != MP_OBJ_NULL) && (mp_obj_is_true(args[PIN_INIT_ARG_VALUE].u_obj))) {
            pin_config.outputLogic = 1U;
        } else {
            pin_config.outputLogic = 0U;
        }
        pin_config.direction = mode == PIN_MODE_IN ? kGPIO_DigitalInput : kGPIO_DigitalOutput;
        pin_config.interruptMode = kGPIO_NoIntmode;

        af_obj = pin_find_af(self, PIN_AF_MODE_ALT5);  // GPIO is always ALT5
        if (af_obj == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("requested AF %d not available for pin %d"), PIN_AF_MODE_ALT5, mode);
        }

        // Generate pad configuration
        if (args[PIN_INIT_ARG_PULL].u_obj != mp_const_none) {
            pull = (uint8_t)mp_obj_get_int(args[PIN_INIT_ARG_PULL].u_obj);
        }

        pad_config = pin_generate_config(pull, mode, drive, self->configRegister);

        // Configure PAD as GPIO
        IOMUXC_SetPinMux(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, pad_config);
        GPIO_PinInit(self->gpio, self->pin, &pin_config);
    }

    return mp_const_none;
}

STATIC void machine_pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(o);
    mp_printf(print, "Pin(%s)", qstr_str(self->name));
}

// pin(id, mode, pull, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_low(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_high(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_obj_call(args[0], (n_args - 1), 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.init(mode, pull, [kwargs])
STATIC mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_init);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
STATIC mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = 3} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the IRQ object.
    uint32_t gpio_nr = GPIO_get_instance(self->gpio);
    uint32_t index = GET_PIN_IRQ_INDEX(gpio_nr, self->pin);
    if (index >= ARRAY_SIZE(MP_STATE_PORT(machine_pin_irq_objects))) {
        mp_raise_ValueError(MP_ERROR_TEXT("IRQ not supported on given Pin"));
    }
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_objects[index]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_pin_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_pin_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(machine_pin_irq_objects[index]) = irq;
    }

    if (n_args > 1 || kw_args->used != 0) {
        // Configure IRQ.
        uint32_t irq_num = self->pin < 16 ? GPIO_combined_low_irqs[gpio_nr] : GPIO_combined_high_irqs[gpio_nr];

        // Disable all IRQs from the affected source while data is updated.
        DisableIRQ(irq_num);
        GPIO_PortDisableInterrupts(self->gpio, 1U << self->pin);

        // Update IRQ data.
        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        irq->flags = 0;
        if (args[ARG_trigger].u_int >= ARRAY_SIZE(IRQ_mapping)) {
            mp_raise_ValueError(MP_ERROR_TEXT("IRQ mode not supported"));
        }
        irq->trigger = IRQ_mapping[args[ARG_trigger].u_int];

        // Enable IRQ if a handler is given.
        if (args[ARG_handler].u_obj != mp_const_none) {
            // Set the pin mode
            GPIO_PinSetInterruptConfig(self->gpio, self->pin, irq->trigger);
            // Enable the specific Pin interrupt
            GPIO_PortEnableInterrupts(self->gpio, 1U << self->pin);
        }
        // Enable LEVEL1 interrupt again
        EnableIRQ(irq_num);
    }

    return MP_OBJ_FROM_PTR(irq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),     MP_ROM_PTR(&machine_pin_irq_obj) },
    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },
    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),      MP_ROM_INT(PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),     MP_ROM_INT(PIN_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(PIN_MODE_OPEN_DRAIN) },

    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(PIN_PULL_UP_100K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP_47K), MP_ROM_INT(PIN_PULL_UP_47K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP_22K), MP_ROM_INT(PIN_PULL_UP_22K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(PIN_PULL_DOWN_100K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_HOLD), MP_ROM_INT(PIN_PULL_HOLD) },

    { MP_ROM_QSTR(MP_QSTR_DRIVE_OFF),  MP_ROM_INT(PIN_DRIVE_OFF) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_0),    MP_ROM_INT(PIN_DRIVE_0) }, // R0 (150 Ohm @3.3V / 260 Ohm @ 1.8V)
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1),    MP_ROM_INT(PIN_DRIVE_1) }, // R0/2
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2),    MP_ROM_INT(PIN_DRIVE_2) }, // R0/3
    { MP_ROM_QSTR(MP_QSTR_DRIVE_3),    MP_ROM_INT(PIN_DRIVE_3) }, // R0/4
    { MP_ROM_QSTR(MP_QSTR_DRIVE_4),    MP_ROM_INT(PIN_DRIVE_4) }, // R0/5
    { MP_ROM_QSTR(MP_QSTR_DRIVE_5),    MP_ROM_INT(PIN_DRIVE_5) }, // R0/6
    { MP_ROM_QSTR(MP_QSTR_DRIVE_6),    MP_ROM_INT(PIN_DRIVE_6) }, // R0/7

    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(2) },

};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);


STATIC mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
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

STATIC const mp_pin_p_t machine_pin_obj_protocol = {
    .ioctl = machine_pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_Pin,
    .print = machine_pin_obj_print,
    .call = machine_pin_obj_call,
    .make_new = mp_pin_make_new,
    .protocol = &machine_pin_obj_protocol,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};

// FIXME: Create actual pin_af type!!!
const mp_obj_type_t machine_pin_af_type = {
    {&mp_type_type},
    .name = MP_QSTR_PinAF,
    .print = machine_pin_obj_print,
    .make_new = mp_pin_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};

STATIC mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t gpio_nr = GPIO_get_instance(self->gpio);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_objects[GET_PIN_IRQ_INDEX(gpio_nr, self->pin)]);
    uint32_t irq_num = self->pin < 16 ? GPIO_combined_low_irqs[gpio_nr] : GPIO_combined_high_irqs[gpio_nr];
    DisableIRQ(irq_num);
    irq->flags = 0;
    irq->trigger = new_trigger;
    // Configure the interrupt.
    GPIO_PinSetInterruptConfig(self->gpio, self->pin, irq->trigger);
    // Enable LEVEL1 interrupt.
    EnableIRQ(irq_num);
    // Enable the specific pin interrupt.
    GPIO_PortEnableInterrupts(self->gpio, 1U << self->pin);
    return 0;
}

STATIC mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t gpio_nr = GPIO_get_instance(self->gpio);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_objects[GET_PIN_IRQ_INDEX(gpio_nr, self->pin)]);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

STATIC const mp_irq_methods_t machine_pin_irq_methods = {
    .trigger = machine_pin_irq_trigger,
    .info = machine_pin_irq_info,
};
