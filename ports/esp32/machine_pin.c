/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Damien P. George
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

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "hal/gpio_ll.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "mphalport.h"
#include "modmachine.h"
#include "machine_pin.h"
#include "machine_rtc.h"
#include "modesp32.h"
#include "genhdr/pins.h"

#if CONFIG_IDF_TARGET_ESP32C3
#include "soc/usb_serial_jtag_reg.h"
#endif

// Used to implement a range of pull capabilities
#define GPIO_PULL_DOWN (1)
#define GPIO_PULL_UP   (2)

#if CONFIG_IDF_TARGET_ESP32
#define GPIO_FIRST_NON_OUTPUT (34)
#elif CONFIG_IDF_TARGET_ESP32S2
#define GPIO_FIRST_NON_OUTPUT (46)
#endif

// Return the gpio_num_t index for a given machine_pin_obj_t pointer.
#define PIN_OBJ_PTR_INDEX(self) ((self) - &machine_pin_obj_table[0])
// Return the machine_pin_obj_t pointer corresponding to a machine_pin_irq_obj_t pointer.
#define PIN_OBJ_PTR_FROM_IRQ_OBJ_PTR(self) ((machine_pin_obj_t *)((uintptr_t)(self) - offsetof(machine_pin_obj_t, irq)))

static const machine_pin_obj_t *machine_pin_find_named(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}

void machine_pins_init(void) {
    static bool did_install = false;
    if (!did_install) {
        gpio_install_isr_service(0);
        did_install = true;
    }
    memset(&MP_STATE_PORT(machine_pin_irq_handler[0]), 0, sizeof(MP_STATE_PORT(machine_pin_irq_handler)));
}

void machine_pins_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_obj_table); ++i) {
        if (machine_pin_obj_table[i].base.type != NULL) {
            gpio_isr_handler_remove(i);
        }
    }
}

static void machine_pin_isr_handler(void *arg) {
    machine_pin_obj_t *self = arg;
    mp_obj_t handler = MP_STATE_PORT(machine_pin_irq_handler)[PIN_OBJ_PTR_INDEX(self)];
    mp_sched_schedule(handler, MP_OBJ_FROM_PTR(self));
    mp_hal_wake_main_task_from_isr();
}

static const machine_pin_obj_t *machine_pin_find(mp_obj_t pin_in) {
    if (mp_obj_is_type(pin_in, &machine_pin_type)) {
        return pin_in;
    }

    // Try to find the pin via integer index into the array of all pins.
    if (mp_obj_is_int(pin_in)) {
        int wanted_pin = mp_obj_get_int(pin_in);
        if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj_table)) {
            const machine_pin_obj_t *self = (machine_pin_obj_t *)&machine_pin_obj_table[wanted_pin];
            if (self->base.type != NULL) {
                return self;
            }
        }
    }

    // Try to find the pin in the board pins dict.
    if (mp_obj_is_str(pin_in)) {
        const machine_pin_obj_t *self = machine_pin_find_named(&machine_pin_board_pins_locals_dict, pin_in);
        if (self && self->base.type != NULL) {
            return self;
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}

gpio_num_t machine_pin_get_id(mp_obj_t pin_in) {
    const machine_pin_obj_t *self = machine_pin_find(pin_in);
    return PIN_OBJ_PTR_INDEX(self);
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "Pin(%u)", PIN_OBJ_PTR_INDEX(self));
}

// pin.init(mode=None, pull=-1, *, value, drive, hold)
static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_drive, ARG_hold };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        { MP_QSTR_hold, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    gpio_num_t index = PIN_OBJ_PTR_INDEX(self);

    // reset the pin to digital if this is a mode-setting init (grab it back from ADC)
    if (args[ARG_mode].u_obj != mp_const_none) {
        if (rtc_gpio_is_valid_gpio(index)) {
            #if !CONFIG_IDF_TARGET_ESP32C3
            rtc_gpio_deinit(index);
            #endif
        }
    }

    #if CONFIG_IDF_TARGET_ESP32C3
    if (index == 18 || index == 19) {
        CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
    }
    #endif

    // configure the pin for gpio
    esp_rom_gpio_pad_select_gpio(index);

    // set initial value (do this before configuring mode/pull)
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        gpio_set_level(index, mp_obj_is_true(args[ARG_value].u_obj));
    }

    // set drive capability (do this before configuring mode)
    if (args[ARG_drive].u_obj != MP_OBJ_NULL && GPIO_IS_VALID_OUTPUT_GPIO(index)) {
        mp_int_t strength = mp_obj_get_int(args[ARG_drive].u_obj);
        if (0 <= strength && strength < GPIO_DRIVE_CAP_MAX) {
            gpio_set_drive_capability(index, strength);
        }
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t pin_io_mode = mp_obj_get_int(args[ARG_mode].u_obj);
        #ifdef GPIO_FIRST_NON_OUTPUT
        if (index >= GPIO_FIRST_NON_OUTPUT && (pin_io_mode & GPIO_MODE_DEF_OUTPUT)) {
            mp_raise_ValueError(MP_ERROR_TEXT("pin can only be input"));
        }
        #endif
        gpio_set_direction(index, pin_io_mode);
    }

    // configure pull
    if (args[ARG_pull].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        int mode = 0;
        if (args[ARG_pull].u_obj != mp_const_none) {
            mode = mp_obj_get_int(args[ARG_pull].u_obj);
        }
        if (mode & GPIO_PULL_DOWN) {
            gpio_pulldown_en(index);
        } else {
            gpio_pulldown_dis(index);
        }
        if (mode & GPIO_PULL_UP) {
            gpio_pullup_en(index);
        } else {
            gpio_pullup_dis(index);
        }
    }

    // configure pad hold
    if (args[ARG_hold].u_obj != MP_OBJ_NULL && GPIO_IS_VALID_OUTPUT_GPIO(index)) {
        // always disable pad hold to apply outstanding config changes
        gpio_hold_dis(index);
        // (re-)enable pad hold if requested
        if (mp_obj_is_true(args[ARG_hold].u_obj)) {
            gpio_hold_en(index);
        }
    }

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted pin object
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
    gpio_num_t index = PIN_OBJ_PTR_INDEX(self);
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(gpio_get_level(index));
    } else {
        // set pin
        gpio_set_level(index, mp_obj_is_true(args[0]));
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

// pin.off()
static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_level(PIN_OBJ_PTR_INDEX(self), 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_level(PIN_OBJ_PTR_INDEX(self), 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING)
static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_wake };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_INTR_POSEDGE | GPIO_INTR_NEGEDGE} },
        { MP_QSTR_wake, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        gpio_num_t index = PIN_OBJ_PTR_INDEX(self);
        mp_obj_t handler = args[ARG_handler].u_obj;
        uint32_t trigger = args[ARG_trigger].u_int;
        mp_obj_t wake_obj = args[ARG_wake].u_obj;

        if ((trigger == GPIO_INTR_LOW_LEVEL || trigger == GPIO_INTR_HIGH_LEVEL) && wake_obj != mp_const_none) {
            mp_int_t wake;
            if (mp_obj_get_int_maybe(wake_obj, &wake)) {
                if (wake < 2 || wake > 7) {
                    mp_raise_ValueError(MP_ERROR_TEXT("bad wake value"));
                }
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("bad wake value"));
            }

            if (machine_rtc_config.wake_on_touch) { // not compatible
                mp_raise_ValueError(MP_ERROR_TEXT("no resources"));
            }

            if (!RTC_IS_VALID_EXT_PIN(index)) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid pin for wake"));
            }

            if (machine_rtc_config.ext0_pin == -1) {
                machine_rtc_config.ext0_pin = index;
            } else if (machine_rtc_config.ext0_pin != index) {
                mp_raise_ValueError(MP_ERROR_TEXT("no resources"));
            }

            machine_rtc_config.ext0_level = trigger == GPIO_INTR_LOW_LEVEL ? 0 : 1;
            machine_rtc_config.ext0_wake_types = wake;
        } else {
            if (machine_rtc_config.ext0_pin == index) {
                machine_rtc_config.ext0_pin = -1;
            }

            if (handler == mp_const_none) {
                handler = MP_OBJ_NULL;
                trigger = 0;
            }
            gpio_isr_handler_remove(index);
            MP_STATE_PORT(machine_pin_irq_handler)[index] = handler;
            gpio_set_intr_type(index, trigger);
            gpio_isr_handler_add(index, machine_pin_isr_handler, (void *)self);
        }
    }

    // return the irq object
    return MP_OBJ_FROM_PTR(&self->irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_board_pins_obj_type,
    MP_QSTR_board,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_board_pins_locals_dict
    );

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&machine_pin_board_pins_obj_type) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_INPUT_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(GPIO_MODE_INPUT_OUTPUT_OD) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_INTR_POSEDGE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_INTR_NEGEDGE) },
    { MP_ROM_QSTR(MP_QSTR_WAKE_LOW), MP_ROM_INT(GPIO_INTR_LOW_LEVEL) },
    { MP_ROM_QSTR(MP_QSTR_WAKE_HIGH), MP_ROM_INT(GPIO_INTR_HIGH_LEVEL) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_0), MP_ROM_INT(GPIO_DRIVE_CAP_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1), MP_ROM_INT(GPIO_DRIVE_CAP_1) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2), MP_ROM_INT(GPIO_DRIVE_CAP_2) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_3), MP_ROM_INT(GPIO_DRIVE_CAP_3) },
};

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;
    gpio_num_t index = PIN_OBJ_PTR_INDEX(self);

    switch (request) {
        case MP_PIN_READ: {
            return gpio_get_level(index);
        }
        case MP_PIN_WRITE: {
            gpio_set_level(index, arg);
            return 0;
        }
    }
    return -1;
}

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

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

/******************************************************************************/
// Pin IRQ object

static mp_obj_t machine_pin_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_pin_irq_obj_t *self = self_in;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    machine_pin_isr_handler((void *)PIN_OBJ_PTR_FROM_IRQ_OBJ_PTR(self));
    return mp_const_none;
}

static mp_obj_t machine_pin_irq_trigger(size_t n_args, const mp_obj_t *args) {
    machine_pin_irq_obj_t *self = args[0];
    gpio_num_t index = PIN_OBJ_PTR_INDEX(PIN_OBJ_PTR_FROM_IRQ_OBJ_PTR(self));
    uint32_t orig_trig = GPIO.pin[index].int_type;
    if (n_args == 2) {
        // set trigger
        gpio_set_intr_type(index, mp_obj_get_int(args[1]));
    }
    // return original trigger value
    return MP_OBJ_NEW_SMALL_INT(orig_trig);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_irq_trigger_obj, 1, 2, machine_pin_irq_trigger);

static const mp_rom_map_elem_t machine_pin_irq_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_trigger), MP_ROM_PTR(&machine_pin_irq_trigger_obj) },
};
static MP_DEFINE_CONST_DICT(machine_pin_irq_locals_dict, machine_pin_irq_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_irq_type,
    MP_QSTR_IRQ,
    MP_TYPE_FLAG_NONE,
    call, machine_pin_irq_call,
    locals_dict, &machine_pin_irq_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t machine_pin_irq_handler[GPIO_PIN_COUNT]);
