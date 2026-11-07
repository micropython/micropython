// machine_pin.c - machine.Pin implementation for GD32F4xx
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/stream.h"
#include "py/obj.h"
#include "extmod/modmachine.h"

#include "extmod/virtpin.h"
#include "gd32f4xx.h"
#include "pin_defs.h"
#include "genhdr/pins.h"

// ---- port helpers ----
uint32_t pin_port_base(uint8_t port_idx) {
    static const uint32_t bases[] = {
        GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI
    };
    if (port_idx >= MP_ARRAY_SIZE(bases)) {
        return GPIOA;
    }
    return bases[port_idx];
}

uint32_t pin_port_rcu(uint8_t port_idx) {
    static const uint32_t rcus[] = {
        RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD, RCU_GPIOE,
        RCU_GPIOF, RCU_GPIOG, RCU_GPIOH, RCU_GPIOI
    };
    if (port_idx >= MP_ARRAY_SIZE(rcus)) {
        return RCU_GPIOA;
    }
    return rcus[port_idx];
}

uint32_t pin_hal_mask(uint8_t num) {
    return (uint32_t)(1U << num);
}

// ---- pin object ----
static void pin_configure(const machine_pin_obj_t *self, uint32_t mode,
                          uint32_t pull, uint32_t otype, uint32_t speed) {
    uint8_t idx = PIN_PORT(self->pin);
    uint32_t port = pin_port_base(idx);
    uint32_t mask = pin_hal_mask(PIN_NUM(self->pin));

    rcu_periph_clock_enable(pin_port_rcu(idx));

    switch (mode) {
        case GPIO_MODE_OUTPUT:
            gpio_mode_set(port, GPIO_MODE_OUTPUT, pull, mask);
            gpio_output_options_set(port, otype, speed, mask);
            break;
        case GPIO_MODE_INPUT:
            gpio_mode_set(port, GPIO_MODE_INPUT, pull, mask);
            break;
        case GPIO_MODE_AF:
            gpio_mode_set(port, GPIO_MODE_AF, pull, mask);
            gpio_output_options_set(port, otype, speed, mask);
            break;
        case GPIO_MODE_ANALOG:
        default:
            gpio_mode_set(port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, mask);
            break;
    }
}

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self,
                                            size_t n_args, const mp_obj_t *pos_args,
                                            mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_MODE_INPUT} },
        { MP_QSTR_pull,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_PUPD_NONE} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    pin_configure(self, args[ARG_mode].u_int, args[ARG_pull].u_int,
                  GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

    if (args[ARG_value].u_obj != mp_const_none) {
        uint32_t port = pin_port_base(PIN_PORT(self->pin));
        uint32_t mask = pin_hal_mask(PIN_NUM(self->pin));
        gpio_bit_write(port, mask,
                       mp_obj_is_true(args[ARG_value].u_obj) ? SET : RESET);
    }

    return mp_const_none;
}

// lookup pin by int or name
static const machine_pin_obj_t *pin_find(mp_obj_t obj) {
    if (mp_obj_is_small_int(obj)) {
        mp_uint_t want = (mp_uint_t)mp_obj_get_int(obj);
        for (const machine_pin_obj_t *p = machine_pin_obj_table; p->pin != PIN_INVALID; p++) {
            if (p->pin == want) {
                return p;
            }
        }
        return NULL;
    }

    qstr name = mp_obj_str_get_qstr(obj);
    for (size_t i = 0; i < machine_pin_board_pins_locals_dict_table_len; i++) {
        if (MP_OBJ_QSTR_VALUE(machine_pin_board_pins_locals_dict_table[i].key) == name) {
            mp_uint_t enc = (mp_uint_t)mp_obj_get_int(
                machine_pin_board_pins_locals_dict_table[i].value);
            for (const machine_pin_obj_t *p = machine_pin_obj_table; p->pin != PIN_INVALID; p++) {
                if (p->pin == enc) {
                    return p;
                }
            }
        }
    }
    return NULL;
}

mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *src = pin_find(all_args[0]);
    if (src == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    machine_pin_obj_t *self = mp_obj_malloc(machine_pin_obj_t, type);
    self->pin = src->pin;

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, all_args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *pos_args,
                                     mp_map_t *kw_args) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    return machine_pin_obj_init_helper(self, n_args - 1, pos_args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_init_obj, 1, machine_pin_obj_init);

// value([v]) - get or set
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t port = pin_port_base(PIN_PORT(self->pin));
    uint32_t mask = pin_hal_mask(PIN_NUM(self->pin));

    if (n_args > 1) {
        gpio_bit_write(port, mask, mp_obj_is_true(args[1]) ? SET : RESET);
        return mp_const_none;
    }
    return mp_obj_new_int(gpio_input_bit_get(port, mask) ? 1 : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_bit_write(pin_port_base(PIN_PORT(self->pin)),
                   pin_hal_mask(PIN_NUM(self->pin)), SET);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_bit_write(pin_port_base(PIN_PORT(self->pin)),
                   pin_hal_mask(PIN_NUM(self->pin)), RESET);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t port = pin_port_base(PIN_PORT(self->pin));
    uint32_t mask = pin_hal_mask(PIN_NUM(self->pin));
    gpio_bit_write(port, mask, gpio_output_bit_get(port, mask) ? RESET : SET);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in,
                              mp_print_kind_t kind) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin(P%c%d)", 'A' + PIN_PORT(self->pin), PIN_NUM(self->pin));
}

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),      MP_ROM_PTR(&machine_pin_obj_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),     MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),        MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),       MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),    MP_ROM_PTR(&machine_pin_toggle_obj) },
    // constants
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(GPIO_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(GPIO_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(GPIO_OTYPE_OD) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),   MP_ROM_INT(GPIO_PUPD_PULLUP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PUPD_PULLDOWN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(GPIO_PUPD_NONE) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);


// ---- virtpin protocol so machine.Signal can wrap a Pin ----
static mp_uint_t machine_pin_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(obj);
    uint32_t port = pin_port_base(PIN_PORT(self->pin));
    uint32_t mask = pin_hal_mask(PIN_NUM(self->pin));

    switch (request) {
        case MP_PIN_READ:
            return gpio_input_bit_get(port, mask) ? 1 : 0;
        case MP_PIN_WRITE:
            gpio_bit_write(port, mask, arg ? SET : RESET);
            return 0;
        case MP_PIN_INPUT:
            rcu_periph_clock_enable(pin_port_rcu(PIN_PORT(self->pin)));
            gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, mask);
            return 0;
        case MP_PIN_OUTPUT:
            rcu_periph_clock_enable(pin_port_rcu(PIN_PORT(self->pin)));
            gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mask);
            gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, mask);
            return 0;
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    print, machine_pin_print,
    protocol, &machine_pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );

// ---- pin object table (generated from pins.csv) ----
const machine_pin_obj_t machine_pin_obj_table[] = {
    #define PIN_ROW(name, port, num) \
        { { &machine_pin_type }, (uint8_t)(((port) << PIN_PORT_SHIFT) | (num)) },
    #include "genhdr/pin_table.h"
    #undef PIN_ROW
    { { &machine_pin_type }, PIN_INVALID },
};

// length of the generated named-pin dictionary
const size_t machine_pin_board_pins_locals_dict_table_len =
    MP_ARRAY_SIZE(machine_pin_board_pins_locals_dict_table);

// helper used by mphalport.h inline wrappers
void mp_hal_pin_write_impl(mp_obj_t pin, int value) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(pin);
    gpio_bit_write(pin_port_base(PIN_PORT(self->pin)),
                   pin_hal_mask(PIN_NUM(self->pin)), value ? SET : RESET);
}

// ---- mp_hal pin abstraction helpers ----
mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    mp_hal_pin_obj_t p;
    if (mp_obj_is_type(obj, &machine_pin_type)) {
        const machine_pin_obj_t *self = MP_OBJ_TO_PTR(obj);
        p.pin = self->pin;
    } else {
        p.pin = (uint8_t)mp_obj_get_int(obj);
    }
    return p;
}

const char *mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    // The generated board table maps names to encodings; search for a name.
    for (size_t i = 0; i < machine_pin_board_pins_locals_dict_table_len; i++) {
        mp_uint_t enc = (mp_uint_t)mp_obj_get_int(
            machine_pin_board_pins_locals_dict_table[i].value);
        if (enc == pin.pin) {
            return qstr_str(MP_OBJ_QSTR_VALUE(
                machine_pin_board_pins_locals_dict_table[i].key));
        }
    }
    return "?";
}
