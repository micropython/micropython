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

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "cy_gpio.h"

#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "machine_pin.h"

#define pin_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

#define MACHINE_PIN_OUT_VAL_UNDEF    0xFFU

uint8_t pin_get_mode(const machine_pin_obj_t *self) {
    uint32_t drive_mode = Cy_GPIO_GetDrivemode(Cy_GPIO_PortToAddr(self->port), self->pin);

    switch (drive_mode) {
        case CY_GPIO_DM_HIGHZ:
        case CY_GPIO_DM_PULLUP_DOWN:
            return GPIO_MODE_IN;

        case CY_GPIO_DM_STRONG:
            return GPIO_MODE_OUT;

        case CY_GPIO_DM_OD_DRIVESLOW:
        case CY_GPIO_DM_PULLUP:
            return GPIO_MODE_OPEN_DRAIN;

        default:
            return GPIO_MODE_NONE;
    }
}

static inline uint8_t pin_get_out_value(const machine_pin_obj_t *self) {
    return Cy_GPIO_ReadOut(Cy_GPIO_PortToAddr(self->port), self->pin);
}

static uint8_t pin_get_pull(const machine_pin_obj_t *self) {
    uint32_t drive_mode = Cy_GPIO_GetDrivemode(Cy_GPIO_PortToAddr(self->port), self->pin);

    switch (drive_mode) {
        case CY_GPIO_DM_PULLUP:
            return GPIO_PULL_UP;

        case CY_GPIO_DM_PULLUP_DOWN:
            return (pin_get_out_value(self) == 1) ? GPIO_PULL_UP : GPIO_PULL_DOWN;

        default:
            return GPIO_PULL_NONE;
    }
}

static inline uint32_t pin_get_drive(const machine_pin_obj_t *self) {
    return Cy_GPIO_GetDriveSel(Cy_GPIO_PortToAddr(self->port), self->pin);
}

static inline void pin_set_drive(const machine_pin_obj_t *self, uint32_t drive) {
    Cy_GPIO_SetDriveSel(Cy_GPIO_PortToAddr(self->port), self->pin, drive);
}

static inline void pin_init_default(const machine_pin_obj_t *self) {
    Cy_GPIO_Pin_FastInit(Cy_GPIO_PortToAddr(self->port), self->pin, CY_GPIO_DM_HIGHZ, 0, HSIOM_SEL_GPIO);
}

qstr get_mode_str(uint8_t mode) {
    switch (mode) {
        case GPIO_MODE_IN:
            return MP_QSTR_IN;
        case GPIO_MODE_OUT:
            return MP_QSTR_OUT;
        case GPIO_MODE_OPEN_DRAIN:
            return MP_QSTR_OPEN_DRAIN;
        default:
            return MP_QSTR_None;
    }
}

qstr get_pull_str(uint8_t pull) {
    switch (pull) {
        case GPIO_PULL_UP:
            return MP_QSTR_PULL_UP;
        case GPIO_PULL_DOWN:
            return MP_QSTR_PULL_DOWN;
        default:
            return MP_QSTR_None;
    }
}

/**
 * Get the PDL drive mode configuration for the given pin mode
 * and pull configuration.
 * The initial value is also validated and set to a coherent
 * value with the pull configuration if not defined by the user.
 */
uint32_t get_drive_mode(uint8_t mode, uint8_t pull, uint8_t *value) {
    uint32_t drive_mode = CY_GPIO_DM_INVALID;

    /* -- Pin.IN -- */
    if (mode == GPIO_MODE_IN) {
        if (*value != MACHINE_PIN_OUT_VAL_UNDEF) {
            mp_printf(&mp_plat_print, "machine.Pin warning: initial value is ignored for Pin.IN mode\n");
        }
        if (pull == GPIO_PULL_NONE) {
            drive_mode = CY_GPIO_DM_HIGHZ;
        } else if (pull == GPIO_PULL_UP) {
            drive_mode = CY_GPIO_DM_PULLUP_DOWN;
            *value = 1;
        } else if (pull == GPIO_PULL_DOWN) {
            drive_mode = CY_GPIO_DM_PULLUP_DOWN;
            *value = 0;
        } else {
            drive_mode = CY_GPIO_DM_INVALID;
        }
        /* -- Pin.OUT -- */
    } else if (mode == GPIO_MODE_OUT) {
        if (*value == MACHINE_PIN_OUT_VAL_UNDEF) {
            *value = 0; // Default to low
        }
        if (pull == GPIO_PULL_NONE) {
            drive_mode = CY_GPIO_DM_STRONG;
        } else {
            drive_mode = CY_GPIO_DM_INVALID;
        }
        /**
         * Pull resistors not configurable for output mode
         */
        /* -- Pin.OPEN_DRAIN -- */
    } else if (mode == GPIO_MODE_OPEN_DRAIN) {
        if (*value == MACHINE_PIN_OUT_VAL_UNDEF) {
            *value = 1; // Default to high
        }
        if (pull == GPIO_PULL_NONE) {
            drive_mode = CY_GPIO_DM_OD_DRIVESLOW;
        } else if (pull == GPIO_PULL_UP) {
            drive_mode = CY_GPIO_DM_PULLUP;
        } else {
            drive_mode = CY_GPIO_DM_INVALID;
        }
        /* -- Pin.None (invalid) -- */
    } else if (mode == GPIO_MODE_NONE) {
        drive_mode = CY_GPIO_DM_INVALID;
    }

    if (drive_mode == CY_GPIO_DM_INVALID) {
        qstr mode_qstr = get_mode_str(mode);
        qstr pull_qstr = get_pull_str(pull);
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("machine.Pin with mode=Pin.%q + pull=Pin.%q is not supported"), mode_qstr, pull_qstr);
    }

    return drive_mode;
}

static inline void assert_mode(uint8_t mode) {
    if (mode != GPIO_MODE_IN && mode != GPIO_MODE_OUT && mode != GPIO_MODE_OPEN_DRAIN) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("machine.Pin invalid mode %d. Valid modes are: Pin.IN, Pin.OUT, Pin.OPEN_DRAIN"), mode);
    }
}

static inline void assert_pull(uint8_t pull) {
    if (pull != GPIO_PULL_NONE && pull != GPIO_PULL_UP && pull != GPIO_PULL_DOWN) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("machine.Pin invalid pull %d. Valid pulls are: Pin.PULL_NONE, Pin.PULL_UP, Pin.PULL_DOWN"), pull);
    }
}

static inline void assert_drive(uint32_t drive) {
    if (drive > CY_GPIO_DRIVE_SEL_7) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("machine.Pin invalid drive %d. Valid drives are: 0-7"), drive);
    }
}

static inline void assert_value(uint8_t value) {
    if (value != 0 && value != 1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("machine.Pin invalid value %d. Valid values are: 0, 1"), value);
    }
}

const machine_pin_obj_t *machine_pin_get_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != MP_OBJ_NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}

const machine_pin_obj_t *machine_pin_get_af_pin(machine_pin_af_unit_t af_unit, machine_pin_af_signal_t af_signal) {
    const mp_map_t *cpu_map = &machine_pin_cpu_pins_locals_dict.map;

    for (size_t i = 0; i < cpu_map->alloc; i++) {
        const mp_map_elem_t *elem = &cpu_map->table[i];
        if (elem->value == MP_OBJ_NULL) {
            continue;
        }

        const machine_pin_obj_t *pin = MP_OBJ_TO_PTR(elem->value);
        for (uint8_t j = 0; j < pin->af_num; j++) {
            const machine_pin_af_obj_t *af = &pin->af[j];
            if (af->unit == af_unit && af->signal == af_signal) {
                return pin;
            }
        }
    }

    return NULL;
}

const machine_pin_obj_t *machine_pin_get_pin_obj(mp_obj_t obj) {
    const machine_pin_obj_t *pin_obj;

    // pin can a pin object already
    if (mp_obj_is_type(obj, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(obj);
    }

    // pin can be a board named pin
    pin_obj = machine_pin_get_named_pin(&machine_pin_board_pins_locals_dict, obj);
    if (pin_obj) {
        return pin_obj;
    }

    // pin can be a cpu named pin
    pin_obj = machine_pin_get_named_pin(&machine_pin_cpu_pins_locals_dict, obj);
    if (pin_obj) {
        return pin_obj;
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(obj));
}

/******************************************************************************/
// MicroPython bindings

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode, ARG_pull, ARG_drive, ARG_value
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_mode,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_pull,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_INT(-1)}},
        {MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}}
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t mode = pin_get_mode(self);
    if (args[ARG_mode].u_obj != mp_const_none) {
        mode = mp_obj_get_uint(args[ARG_mode].u_obj);
        assert_mode(mode);
    }

    uint8_t pull;
    if (args[ARG_pull].u_obj == mp_const_none) {
        pull = GPIO_PULL_NONE;
    } else {
        mp_int_t p = mp_obj_get_int(args[ARG_pull].u_obj);
        if (p == -1) {
            pull = pin_get_pull(self);
        } else {
            pull = p;
        }
    }
    assert_pull(pull);

    uint8_t value = MACHINE_PIN_OUT_VAL_UNDEF;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
        assert_value(value);
    }

    uint32_t drive = pin_get_drive(self);
    if (args[ARG_drive].u_obj != mp_const_none) {
        drive = mp_obj_get_uint(args[ARG_drive].u_obj);
        assert_drive(drive);
    }

    mp_hal_pin_config(self, mode, pull, value);
    pin_set_drive(self, drive);

    return mp_const_none;
}

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 6, true);

    const machine_pin_obj_t *self = machine_pin_get_pin_obj(args[0]);
    pin_init_default(self);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args); // skipping "id" as an arg as it is a part of self*.
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_init_obj, 1, machine_pin_obj_init);

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin(Pin.cpu.%q", self->name);

    uint8_t mode = pin_get_mode(self);
    qstr mode_qst = get_mode_str(mode);
    if (mode_qst != MP_QSTR_None) {
        mp_printf(print, ", mode=Pin.%q", mode_qst);
    }

    uint8_t pull = pin_get_pull(self);
    qstr pull_qst = get_pull_str(pull);
    if (pull_qst != MP_QSTR_None) {
        mp_printf(print, ", pull=Pin.%q", pull_qst);
    }
    mp_print_str(print, ")");
}

static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_low(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_high(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_hal_pin_read(self)) {
        mp_hal_pin_low(self);
    } else {
        mp_hal_pin_high(self);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static mp_obj_t machine_pin_mode(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(pin_get_mode(self));
    } else {
        uint8_t mode = mp_obj_get_uint(args[1]);
        assert_mode(mode);
        mp_hal_pin_config(self, mode, GPIO_PULL_NONE, MACHINE_PIN_OUT_VAL_UNDEF);
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_mode_obj, 1, 2, machine_pin_mode);

static mp_obj_t machine_pin_pull(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        uint8_t pull = pin_get_pull(self);
        if (pull == GPIO_PULL_NONE) {
            return mp_const_none;
        } else {
            return MP_OBJ_NEW_SMALL_INT(pull);
        }
    } else {
        uint8_t pull = (args[1] == mp_const_none) ? GPIO_PULL_NONE : mp_obj_get_uint(args[1]);
        assert_pull(pull);
        mp_hal_pin_config(self, pin_get_mode(self), pull, MACHINE_PIN_OUT_VAL_UNDEF);
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_pull_obj, 1, 2, machine_pin_pull);


static mp_obj_t machine_pin_drive(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(pin_get_drive(self));
    } else {
        uint32_t drive = mp_obj_get_uint(args[1]);
        assert_drive(drive);
        pin_set_drive(self, drive);
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_drive_obj, 1, 2, machine_pin_drive);

extern mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

/**
 * void machine_pin_deinit_all(void) {
 *    Not implemented.
 *
 *    Rationale:
 *    There is no PDL API to deinitialize a single pin, only at the port level via
 *    void Cy_GPIO_Port_Deinit(GPIO_PRT_Type *base).
 *    If we deinitialize an entire port, we will end up deinitializing other
 *    pins that are used by MicroPython but not defined at the user level,
 *    such as the REPL UART interface pins. Thus, breaking basic core functionalities.
 *
 *    This means that after a soft reset, pins will retain their
 *    configuration, which is not ideal since the system is not starting in
 *    the same state.
 *    This should not be a problem since pins can be reconfigured if reused,
 *    and if not reused, they won't be in scope after reset.
 *    The pin itself is not deinitialized, but its IRQ (if enabled) will be cleared,
 *    disabled, and deinitialized upon soft reset. That should be sufficient to
 *    prevent unexpected behavior.
 *
 *    Discarded solutions:
 *    We could reset pins to default values, but we don't know if there is
 *    a single valid default configuration for all pins. Other initial
 *    value tracking mechanisms would also overcomplicate the code.
 *    Therefore, at the moment, such solutions don't seem worth it.
 * }
 */

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

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_obj_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),  MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode),    MP_ROM_PTR(&machine_pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull),    MP_ROM_PTR(&machine_pin_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive),   MP_ROM_PTR(&machine_pin_drive_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),     MP_ROM_PTR(&machine_pin_irq_obj) },

    // class attributes
    #if MICROPY_PY_MACHINE_PIN_BOARD_NUM_ENTRIES > 0
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&pin_board_pins_obj_type) },
    #endif
    #if MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES > 0
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&pin_cpu_pins_obj_type) },
    #endif

    // Const
    { MP_ROM_QSTR(MP_QSTR_IN),                      MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),                     MP_ROM_INT(GPIO_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),              MP_ROM_INT(GPIO_MODE_OPEN_DRAIN) },

    { MP_ROM_QSTR(MP_QSTR_PULL_UP),                 MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),               MP_ROM_INT(GPIO_PULL_DOWN) },

    { MP_ROM_QSTR(MP_QSTR_DRIVE_0),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_1) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_2) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_3),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_3) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_4),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_4) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_5),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_5) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_6),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_6) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_7),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_7) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),             MP_ROM_INT(CY_GPIO_INTR_FALLING)},
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),              MP_ROM_INT(CY_GPIO_INTR_RISING)},
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

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
