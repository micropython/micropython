// micropython includes
#include "py/mphal.h"
#include "py/runtime.h"


// port-specific includes
#include "modmachine.h"
#include "machine_pin.h"
#include "extmod/virtpin.h"
#include "mplogger.h"
#include "cyhal.h"

enum {GPIO_MODE_NONE = 0, GPIO_MODE_IN, GPIO_MODE_OUT, GPIO_MODE_OPEN_DRAIN};

enum {GPIO_PULL_NONE = 0, GPIO_PULL_UP, GPIO_PULL_DOWN};

typedef struct _machine_pin_io_obj_t {
    mp_obj_base_t base;
    uint32_t pin_addr;
    char *pin_name;
    machine_pin_phy_obj_t *pin_phy;
    uint8_t mode;
    uint8_t drive;
    uint8_t pull;
} machine_pin_io_obj_t;

machine_pin_io_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, PIN_P0_0, "P0_0"},
    {{&machine_pin_type}, PIN_P0_1, "P0_1"},
    {{&machine_pin_type}, PIN_P0_2, "P0_2"},
    {{&machine_pin_type}, PIN_P0_3, "P0_3"},
    {{&machine_pin_type}, PIN_P0_4, "P0_4"},
    {{&machine_pin_type}, PIN_P0_5, "P0_5"},

    {{&machine_pin_type}, PIN_P1_0, "P1_0"},
    {{&machine_pin_type}, PIN_P1_1, "P1_1"},
    {{&machine_pin_type}, PIN_P1_2, "P1_2"},
    {{&machine_pin_type}, PIN_P1_3, "P1_3"},
    {{&machine_pin_type}, PIN_P1_4, "P1_4"},
    {{&machine_pin_type}, PIN_P1_5, "P1_5"},

    {{&machine_pin_type}, PIN_P2_0, "P2_0"},
    {{&machine_pin_type}, PIN_P2_1, "P2_1"},
    {{&machine_pin_type}, PIN_P2_2, "P2_2"},
    {{&machine_pin_type}, PIN_P2_3, "P2_3"},
    {{&machine_pin_type}, PIN_P2_4, "P2_4"},
    {{&machine_pin_type}, PIN_P2_5, "P2_5"},
    {{&machine_pin_type}, PIN_P2_6, "P2_6"},
    {{&machine_pin_type}, PIN_P2_7, "P2_7"},

    {{&machine_pin_type}, PIN_P3_0, "P3_0"},
    {{&machine_pin_type}, PIN_P3_1, "P3_1"},
    {{&machine_pin_type}, PIN_P3_2, "P3_2"},
    {{&machine_pin_type}, PIN_P3_3, "P3_3"},
    {{&machine_pin_type}, PIN_P3_4, "P3_4"},
    {{&machine_pin_type}, PIN_P3_5, "P3_5"},

    {{&machine_pin_type}, PIN_P4_0, "P4_0"},
    {{&machine_pin_type}, PIN_P4_1, "P4_1"},

    {{&machine_pin_type}, PIN_P5_0, "P5_0"},
    {{&machine_pin_type}, PIN_P5_1, "P5_1"},
    {{&machine_pin_type}, PIN_P5_2, "P5_2"},
    {{&machine_pin_type}, PIN_P5_3, "P5_3"},
    {{&machine_pin_type}, PIN_P5_4, "P5_4"},
    {{&machine_pin_type}, PIN_P5_5, "P5_5"},
    {{&machine_pin_type}, PIN_P5_6, "P5_6"},
    {{&machine_pin_type}, PIN_P5_7, "P5_7"},

    {{&machine_pin_type}, PIN_P6_0, "P6_0"},
    {{&machine_pin_type}, PIN_P6_1, "P6_1"},
    {{&machine_pin_type}, PIN_P6_2, "P6_2"},
    {{&machine_pin_type}, PIN_P6_3, "P6_3"},
    {{&machine_pin_type}, PIN_P6_4, "P6_4"},
    {{&machine_pin_type}, PIN_P6_5, "P6_5"},
    {{&machine_pin_type}, PIN_P6_6, "P6_6"},
    {{&machine_pin_type}, PIN_P6_7, "P6_7"},

    {{&machine_pin_type}, PIN_P7_0, "P7_0"},
    {{&machine_pin_type}, PIN_P7_1, "P7_1"},
    {{&machine_pin_type}, PIN_P7_2, "P7_2"},
    {{&machine_pin_type}, PIN_P7_3, "P7_3"},
    {{&machine_pin_type}, PIN_P7_4, "P7_4"},
    {{&machine_pin_type}, PIN_P7_5, "P7_5"},
    {{&machine_pin_type}, PIN_P7_6, "P7_6"},
    {{&machine_pin_type}, PIN_P7_7, "P7_7"},

    {{&machine_pin_type}, PIN_P8_0, "P8_0"},
    {{&machine_pin_type}, PIN_P8_1, "P8_1"},
    {{&machine_pin_type}, PIN_P8_2, "P8_2"},
    {{&machine_pin_type}, PIN_P8_3, "P8_3"},
    {{&machine_pin_type}, PIN_P8_4, "P8_4"},
    {{&machine_pin_type}, PIN_P8_5, "P8_5"},
    {{&machine_pin_type}, PIN_P8_6, "P8_6"},
    {{&machine_pin_type}, PIN_P8_7, "P8_7"},

    {{&machine_pin_type}, PIN_P9_0, "P9_0"},
    {{&machine_pin_type}, PIN_P9_1, "P9_1"},
    {{&machine_pin_type}, PIN_P9_2, "P9_2"},
    {{&machine_pin_type}, PIN_P9_3, "P9_3"},
    {{&machine_pin_type}, PIN_P9_4, "P9_4"},
    {{&machine_pin_type}, PIN_P9_5, "P9_5"},
    {{&machine_pin_type}, PIN_P9_6, "P9_6"},
    {{&machine_pin_type}, PIN_P9_7, "P9_7"},

    {{&machine_pin_type}, PIN_P10_0, "P10_0"},
    {{&machine_pin_type}, PIN_P10_1, "P10_1"},
    {{&machine_pin_type}, PIN_P10_2, "P10_2"},
    {{&machine_pin_type}, PIN_P10_3, "P10_3"},
    {{&machine_pin_type}, PIN_P10_4, "P10_4"},
    {{&machine_pin_type}, PIN_P10_5, "P10_5"},
    {{&machine_pin_type}, PIN_P10_6, "P10_6"},
    {{&machine_pin_type}, PIN_P10_7, "P10_7"},

    {{&machine_pin_type}, PIN_P11_0, "P11_0"},
    {{&machine_pin_type}, PIN_P11_1, "P11_1"},
    {{&machine_pin_type}, PIN_P11_2, "P11_2"},
    {{&machine_pin_type}, PIN_P11_3, "P11_3"},
    {{&machine_pin_type}, PIN_P11_4, "P11_4"},
    {{&machine_pin_type}, PIN_P11_5, "P11_5"},
    {{&machine_pin_type}, PIN_P11_6, "P11_6"},
    {{&machine_pin_type}, PIN_P11_7, "P11_7"},

    {{&machine_pin_type}, PIN_P12_0, "P12_0"},
    {{&machine_pin_type}, PIN_P12_1, "P12_1"},
    {{&machine_pin_type}, PIN_P12_2, "P12_2"},
    {{&machine_pin_type}, PIN_P12_3, "P12_3"},
    {{&machine_pin_type}, PIN_P12_4, "P12_4"},
    {{&machine_pin_type}, PIN_P12_5, "P12_5"},
    {{&machine_pin_type}, PIN_P12_6, "P12_6"},
    {{&machine_pin_type}, PIN_P12_7, "P12_7"},

    {{&machine_pin_type}, PIN_P13_0, "P13_0"},
    {{&machine_pin_type}, PIN_P13_1, "P13_1"},
    {{&machine_pin_type}, PIN_P13_2, "P13_2"},
    {{&machine_pin_type}, PIN_P13_3, "P13_3"},
    {{&machine_pin_type}, PIN_P13_4, "P13_4"},
    {{&machine_pin_type}, PIN_P13_5, "P13_5"},
    {{&machine_pin_type}, PIN_P13_6, "P13_6"},
    {{&machine_pin_type}, PIN_P13_7, "P13_7"},

    {{&machine_pin_type}, PIN_USBDP, "USBDP"},
    {{&machine_pin_type}, PIN_USBDM, "USBDM"},
};

// Function definitions
// helper function to translate pin_name(string) into machine_pin_io_obj_t index.
int pin_find(mp_obj_t pin) {
    int wanted_pin = -1;
    if (mp_obj_is_small_int(pin)) {
        // Pin defined by the index of pin table
        wanted_pin = mp_obj_get_int(pin);
    } else if (mp_obj_is_str(pin)) {
        // Search by name
        size_t slen;
        const char *s = mp_obj_str_get_data(pin, &slen);
        for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_obj); i++) {
            if (slen == strlen(machine_pin_obj[i].pin_name) && strncmp(s, machine_pin_obj[i].pin_name, slen) == 0) {
                wanted_pin = i;
                break;
            }
        }
    }

    if (!(0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin: Pin not defined!"));
    }

    return wanted_pin;
}

// helper function to translate pin_name(string) into machine_pin_io_obj_t->pin_addr
int pin_addr_by_name(mp_obj_t pin) {
    if (mp_obj_is_str(pin)) {
        return machine_pin_obj[pin_find(pin)].pin_addr;
    } else {
        return -1; // expecting a str as input
    }
}

#define MAX_PIN_IO MP_ARRAY_SIZE(machine_pin_phy_obj)
machine_pin_io_obj_t *pin_io[MAX_PIN_IO] = {NULL};

static inline machine_pin_io_obj_t *pin_io_allocate(void) {
    uint16_t i;
    for (i = 0; i < MAX_PIN_IO; i++) {
        if (pin_io[i] == NULL) {
            break;
        }
    }
    pin_io[i] = mp_obj_malloc(machine_pin_io_obj_t, &machine_pin_type);
    return pin_io[i];
}

STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_io_obj_t *self = self_in;

    if (n_args == 0) {
        if (self->mode == GPIO_MODE_IN) {
            return MP_OBJ_NEW_SMALL_INT(cyhal_gpio_read(self->pin_phy->addr));
        } else {
            return mp_const_none;
        }
    } else {
        if (self->mode != GPIO_MODE_IN) {
            bool value = mp_obj_is_true(args[0]);
            cyhal_gpio_write(self->pin_phy->addr, value);
        }
    }

    return mp_const_none;
}

// Pin.print()
STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mplogger_print("machine pin print\n");

    machine_pin_io_obj_t *self = self_in;

    qstr mode_qstr;
    switch (self->mode) {
        case GPIO_MODE_IN:
            mode_qstr = MP_QSTR_IN;
            break;

        case GPIO_MODE_OUT:
            mode_qstr = MP_QSTR_OUT;
            break;

        case GPIO_MODE_OPEN_DRAIN:
            mode_qstr = MP_QSTR_OPEN_DRAIN;
            break;

        case GPIO_MODE_NONE:
            mode_qstr = MP_QSTR_None;
            break;
    }

    qstr pull_qstr;
    switch (self->pull) {
        case GPIO_PULL_UP:
            pull_qstr = MP_QSTR_PULL_UP;
            break;

        case GPIO_PULL_DOWN:
            pull_qstr = MP_QSTR_PULL_DOWN;
            break;

        case GPIO_PULL_NONE:
            pull_qstr = MP_QSTR_None;
            break;
    }

    mp_printf(print, "Pin:%u or %s, Mode=%q, Pull=%q", self->pin_phy->addr, self->pin_phy->name, mode_qstr, pull_qstr);
}

static cyhal_gpio_direction_t mp_to_cy_get_gpio_direction(uint8_t mode) {
    cyhal_gpio_direction_t direction;

    switch (mode) {
        case GPIO_MODE_IN:
            direction = CYHAL_GPIO_DIR_INPUT;
            break;

        case GPIO_MODE_OUT:
        case GPIO_MODE_OPEN_DRAIN:
            direction = CYHAL_GPIO_DIR_OUTPUT;
            break;

        case GPIO_MODE_NONE:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Pin mode is a required argument\n"));
            break;
    }
    return direction;
}

static cyhal_gpio_drive_mode_t mp_to_cy_get_gpio_drive(uint8_t mode, uint8_t pull) {
    cyhal_gpio_drive_mode_t cy_drive;

    // drive modes explained here: https://community.infineon.com/t5/Knowledge-Base-Articles/Drive-Modes-in-PSoC-GPIO/ta-p/248470
    // see app note: https://www.infineon.com/dgdl/Infineon-AN2094_PSoC_1_Getting_Started_With_GPIO-ApplicationNotes-v12_00-EN.pdf?fileId=8ac78c8c7cdc391c017d072966174e13&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-application_note

    switch (mode) {
        case GPIO_MODE_IN: {
            // mpy_drive value is ignored. Only relevant for outputs.
            switch (pull) {
                case GPIO_PULL_UP:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLUP;
                    break;

                case GPIO_PULL_DOWN:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLDOWN;
                    break;

                case GPIO_PULL_NONE:
                default:
                    cy_drive = CYHAL_GPIO_DRIVE_PULL_NONE;
                    break;
            }
        }
        break;

        case GPIO_MODE_OUT: {
            switch (pull) {
                case GPIO_PULL_UP:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLUP;
                    break;

                case GPIO_PULL_DOWN:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLDOWN;
                    break;

                case GPIO_PULL_NONE:
                default:
                    // cyhal will set drive strong for output if PULL NONE.
                    cy_drive = CYHAL_GPIO_DRIVE_PULL_NONE;
                    break;
            }
        }
        break;

        case GPIO_MODE_OPEN_DRAIN:
            // mpy_drive is ignored. Not relevant for open drain output.
            // mpy_pull is ignored. Not configurable for open drain output.
            cy_drive = CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW;
            break;
    }

    return cy_drive;
}

static bool machine_pin_is_inited(machine_pin_io_obj_t *self) {
    // The mode is a mandatory argument for initialization
    // Thus, if other than NONE, the cyhal_gpio object is already initialized.
    if (self->mode != GPIO_MODE_NONE) {
        return true;
    }
    return false;
}

// helper function to parse given initial params and invoke HAL-level GPIO functions
STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_io_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mplogger_print("init helper function called\n");

    enum {ARG_mode, ARG_pull, ARG_value, ARG_drive}; // , ARG_alt};
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_mode,  MP_ARG_OBJ,                     {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_pull,  MP_ARG_OBJ,                     {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ,    {.u_rom_obj = MP_ROM_NONE}}
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool pin_already_inited = machine_pin_is_inited(self);

    bool value = false; // Default low. Initial value irrelevant if not specified.
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    self->mode = GPIO_MODE_NONE;
    if (args[ARG_mode].u_obj != mp_const_none) {
        self->mode = mp_obj_get_int(args[ARG_mode].u_obj);
    }

    self->pull = GPIO_PULL_NONE;
    if (args[ARG_pull].u_obj != mp_const_none) {
        self->pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }

    cyhal_gpio_direction_t direction = mp_to_cy_get_gpio_direction(self->mode);
    cyhal_gpio_drive_mode_t drive = mp_to_cy_get_gpio_drive(self->mode, self->pull);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    if (pin_already_inited) {
        result = cyhal_gpio_configure(self->pin_phy->addr, direction, drive);
    } else {
        cyhal_gpio_free(self->pin_addr); // TODO: remove
        result = cyhal_gpio_init(self->pin_phy->addr, direction, drive, value);
    }
    mplogger_print("Direction: %d, Drive:%d, Value:%d\n", direction, drive, value);

    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("machine pin error: Init unsuccessful\n"));
    }
    return mp_const_none;
}

// Machine Pin methods - port-specific definitions
// Pin constructor(id,mode,pull,value=value,drive=drive,alt=alt)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mplogger_print("%q constructor invoked\n", MP_QSTR_Pin);

    mp_arg_check_num(n_args, n_kw, 1, 6, true);

    machine_pin_phy_obj_t *pin_phy = pin_phy_realloc(args[0], PIN_PHY_FUNC_DIO);

    machine_pin_io_obj_t *self = pin_io_allocate();
    if (self == NULL) {
        return mp_const_none;
    }

    self->pin_phy = pin_phy;

    // go into param arg parsing if args apart from "id" are provided (for ex. pin.Mode, pin.PULL etc)
    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args); // skipping "id" as an arg as it is a part of self*.
    }
    return MP_OBJ_FROM_PTR(self);
}

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// instantiates obj of Pin class
// Pin.init(mode,pull,value=value,drive=drive,alt=alt)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_init_obj, 1, machine_pin_obj_init);


// Pin.toggle()
STATIC mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mode != GPIO_MODE_IN) {
        cyhal_gpio_toggle(self->pin_phy->addr);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// Pin.high()
STATIC mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mode != GPIO_MODE_IN) {
        cyhal_gpio_write(self->pin_phy->addr, true);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// Pin.low()
STATIC mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mode != GPIO_MODE_IN) {
        cyhal_gpio_write(self->pin_phy->addr, false);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods
    { MP_ROM_QSTR(MP_QSTR___name__),                MP_ROM_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_init),                    MP_ROM_PTR(&machine_pin_obj_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),                  MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),                   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),                     MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),                    MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),                     MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),                      MP_ROM_PTR(&machine_pin_low_obj) },

    // Const
    { MP_ROM_QSTR(MP_QSTR_IN),                      MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),                     MP_ROM_INT(GPIO_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),              MP_ROM_INT(GPIO_MODE_OPEN_DRAIN) },

    { MP_ROM_QSTR(MP_QSTR_PULL_UP),                 MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),               MP_ROM_INT(GPIO_PULL_DOWN) }
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_io_obj_t *self = self_in;
    switch (request) {
        case MP_PIN_READ: {
            return cyhal_gpio_read(self->pin_addr);
        }
        case MP_PIN_WRITE: {
            cyhal_gpio_write(self->pin_addr, arg);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_pin_p_t pin_pin_p = {
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
