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

uint8_t pin_get_mode(const machine_pin_obj_t *self) {
    uint32_t drive_mode = Cy_GPIO_GetDrivemode(Cy_GPIO_PortToAddr(self->port), self->pin);

    switch (drive_mode) {
        case CY_GPIO_DM_HIGHZ:
        case CY_GPIO_DM_PULLUP:
        case CY_GPIO_DM_PULLDOWN:
        case CY_GPIO_DM_PULLUP_DOWN:
            return GPIO_MODE_IN;

        case CY_GPIO_DM_STRONG_IN_OFF:
        case CY_GPIO_DM_PULLUP_IN_OFF:
        case CY_GPIO_DM_PULLDOWN_IN_OFF:
        case CY_GPIO_DM_PULLUP_DOWN_IN_OFF:
            return GPIO_MODE_OUT;

        case CY_GPIO_DM_OD_DRIVESLOW_IN_OFF:
        /* These 2 modes are not configurable
        by the user but they could be set by
        at C level */
        case CY_GPIO_DM_OD_DRIVESLOW:
        case CY_GPIO_DM_OD_DRIVESHIGH:
            return GPIO_MODE_OPEN_DRAIN;

        default:
            return GPIO_MODE_NONE;
    }
}

static uint8_t pin_get_pull(const machine_pin_obj_t *self) {
    uint32_t drive_mode = Cy_GPIO_GetDrivemode(Cy_GPIO_PortToAddr(self->port), self->pin);

    switch (drive_mode) {
        case CY_GPIO_DM_PULLUP:
        case CY_GPIO_DM_PULLUP_IN_OFF:
            return GPIO_PULL_UP;

        case CY_GPIO_DM_PULLDOWN:
        case CY_GPIO_DM_PULLDOWN_IN_OFF:
            return GPIO_PULL_DOWN;

        case CY_GPIO_DM_PULLUP_DOWN:
        case CY_GPIO_DM_PULLUP_DOWN_IN_OFF:
            return GPIO_PULL_UP_DOWN;

        default:
            return GPIO_PULL_NONE;
    }
}

static uint32_t get_drive_mode(uint8_t mode, uint8_t pull) {
    uint32_t drive_mode = CY_GPIO_DM_INVALID;

    if (mode == GPIO_MODE_IN) {
        if (pull == GPIO_PULL_UP) {
            drive_mode = CY_GPIO_DM_PULLUP;
        } else if (pull == GPIO_PULL_DOWN) {
            drive_mode = CY_GPIO_DM_PULLDOWN;
        } else if (pull == GPIO_PULL_UP_DOWN) {
            drive_mode = CY_GPIO_DM_PULLUP_DOWN;
        } else {
            drive_mode = CY_GPIO_DM_HIGHZ;
        }
        // TODO: Check if this will be used.
        // CY_GPIO_DM_STRONG ??
    } else if (mode == GPIO_MODE_OUT) {
        if (pull == GPIO_PULL_UP) {
            drive_mode = CY_GPIO_DM_PULLUP_IN_OFF;
        } else if (pull == GPIO_PULL_DOWN) {
            drive_mode = CY_GPIO_DM_PULLDOWN_IN_OFF;
        } else if (pull == GPIO_PULL_UP_DOWN) {
            drive_mode = CY_GPIO_DM_PULLUP_DOWN_IN_OFF;
        } else {
            drive_mode = CY_GPIO_DM_STRONG_IN_OFF;
        }
    } else if (mode == GPIO_MODE_OPEN_DRAIN) {
        drive_mode = CY_GPIO_DM_OD_DRIVESLOW_IN_OFF;
        // TODO: Check if we support other open drain modes
        // CY_GPIO_DM_OD_DRIVESLOW
        // CY_GPIO_DM_OD_DRIVESHIGH
    } else if (mode == GPIO_MODE_NONE) {
        drive_mode = CY_GPIO_DM_INVALID;
    }

    return drive_mode;
}

static uint8_t get_validated_initial_value(uint8_t mode, uint8_t pull, int8_t value) {
    // No value was selected by the user,
    // Select it coherent with the pull resistor
    // configuration and low by default
    if (value == -1) {
        return (pull == GPIO_PULL_UP) ? 1 : 0;
    }

    if (mode == GPIO_MODE_IN) {
        if (value == 0 && pull == GPIO_PULL_UP) {
            mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("machine.Pin incompatible configuration. Input pull-up can not be initialized low.\n"));
        } else if (value == 1 && pull == GPIO_PULL_DOWN) {
            mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("machine.Pin incompatible configuration. Input pull-down can not be initialized high.\n"));
        } else if (pull == GPIO_PULL_NONE || pull == GPIO_PULL_UP_DOWN) {
            value = 0; // Default to low
            mp_printf(&mp_plat_print, "machine.Pin warning: Initial value is undefined for input pull-none configuration.\n");
        }
        return value;
    }

    return value;
}

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode, ARG_pull, ARG_drive, ARG_value
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_mode,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_pull,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}}
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t mode = GPIO_MODE_NONE;
    if (args[ARG_mode].u_obj != mp_const_none) {
        mode = mp_obj_get_uint(args[ARG_mode].u_obj);
    }

    uint8_t pull = GPIO_PULL_NONE;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_uint(args[ARG_pull].u_obj);
    }

    int8_t value = -1;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    uint32_t drive = CY_GPIO_DRIVE_SEL_0;
    if (args[ARG_drive].u_obj != mp_const_none) {
        drive = mp_obj_get_uint(args[ARG_drive].u_obj);
    }

    cy_stc_gpio_pin_config_t pin_config = {
        .outVal = get_validated_initial_value(mode, pull, value),
        .driveMode = get_drive_mode(mode, pull),
        .hsiom = HSIOM_SEL_GPIO,
        .intEdge = CY_GPIO_INTR_DISABLE,
        .intMask = 0UL,
        .vtrip = CY_GPIO_VTRIP_CMOS,
        .slewRate = CY_GPIO_SLEW_FAST,
        .driveSel = drive,
        .vregEn = 0UL,
        .ibufMode = 0UL,
        .vtripSel = 0UL,
        .vrefSel = 0UL,
        .vohSel = 0UL,
        .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
        .nonSec = 1,
    };

    cy_en_gpio_status_t rslt = Cy_GPIO_Pin_Init(Cy_GPIO_PortToAddr(self->port), self->pin, &pin_config);
    pin_assert_raise_val("Pin initialization failed (PSE PDL error code: %lx)", rslt);

    return mp_const_none;
}

const machine_pin_obj_t *machine_pin_get_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != MP_OBJ_NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
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

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 6, true);

    const machine_pin_obj_t *self = machine_pin_get_pin_obj(args[0]);

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
    qstr mode_qst = MP_QSTRnull;
    if (mode == GPIO_MODE_IN) {
        mode_qst = MP_QSTR_IN;
    } else if (mode == GPIO_MODE_OUT) {
        mode_qst = MP_QSTR_OUT;
    } else if (mode == GPIO_MODE_OPEN_DRAIN) {
        mode_qst = MP_QSTR_OPEN_DRAIN;
    }
    if (mode_qst != MP_QSTRnull) {
        mp_printf(print, ", mode=Pin.%q", mode_qst);
    }

    uint8_t pull = pin_get_pull(self);
    qstr pull_qst = MP_QSTRnull;
    if (pull == GPIO_PULL_UP) {
        pull_qst = MP_QSTR_PULL_UP;
    } else if (pull == GPIO_PULL_DOWN) {
        pull_qst = MP_QSTR_PULL_DOWN;
    } else if (pull == GPIO_PULL_UP_DOWN) {
        pull_qst = MP_QSTR_PULL_UP_DOWN;
    }

    if (pull_qst != MP_QSTRnull) {
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


extern mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    // TODO: Placeholder.
    return 0;
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
    { MP_ROM_QSTR(MP_QSTR_PULL_UP_DOWN),            MP_ROM_INT(GPIO_PULL_UP_DOWN) },

    { MP_ROM_QSTR(MP_QSTR_DRIVE_0),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_1) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_2) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_3),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_3) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_4),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_4) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_5),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_5) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_6),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_6) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_7),                MP_ROM_INT(CY_GPIO_DRIVE_SEL_7) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),             MP_ROM_INT(GPIO_IRQ_FALLING)},
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),              MP_ROM_INT(GPIO_IRQ_RISING)},
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
