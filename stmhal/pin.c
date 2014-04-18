#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"

// Usage Model:
//
// All Board Pins are predefined as pyb.Pin.board.Name
//
//   x1_pin = pyb.Pin.board.X1
//
//   g = pyb.gpio(pyb.Pin.board.X1, 0)
//
// CPU pins which correspond to the board pins are available
// as pyb.cpu.Name. For the CPU pins, the names are the port letter
// followed by the pin number. On the PYBV4, pyb.Pin.board.X1 and
// pyb.Pin.cpu.B6 are the same pin.
//
// You can also use strings:
//
//   g = pyb.gpio('X1', 0)
//
// Users can add their own names:
//
//   pyb.Pin("LeftMotorDir", pyb.Pin.cpu.C12)
//   g = pyb.gpio("LeftMotorDir", 0)
//
// and can query mappings
//
//   pin = pyb.Pin("LeftMotorDir");
//
// Users can also add their own mapping function:
//
//   def MyMapper(pin_name):
//      if pin_name == "LeftMotorDir":
//          return pyb.Pin.cpu.A0
//
//   pyb.Pin.mapper(MyMapper)
//
// So, if you were to call: pyb.gpio("LeftMotorDir", 0)
// then "LeftMotorDir" is passed directly to the mapper function.
//
// To summarize, the following order determines how things get mapped into
// an ordinal pin number:
//
// 1 - Directly specify a pin object
// 2 - User supplied mapping function
// 3 - User supplied mapping (object must be usable as a dictionary key)
// 4 - Supply a string which matches a board pin
// 5 - Supply a string which matches a CPU port/pin
//
// You can set pyb.Pin.debug(True) to get some debug information about
// how a particular object gets mapped to a pin.

// Pin class variables
STATIC mp_obj_t pin_class_mapper;
STATIC mp_obj_t pin_class_map_dict;
STATIC bool pin_class_debug;

void pin_init(void) {
    pin_class_mapper = MP_OBJ_NULL;
    pin_class_map_dict = MP_OBJ_NULL;
    pin_class_debug = false;
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
const pin_obj_t *pin_find(mp_obj_t user_obj) {
    const pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (MP_OBJ_IS_TYPE(user_obj, &pin_type)) {
        pin_obj = user_obj;
        if (pin_class_debug) {
            printf("Pin map passed pin ");
            mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    if (pin_class_mapper != MP_OBJ_NULL) {
        pin_obj = mp_call_function_1(pin_class_mapper, user_obj);
        if (pin_obj != mp_const_none) {
            if (!MP_OBJ_IS_TYPE(pin_obj, &pin_type)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Pin.mapper didn't return a Pin object"));
            }
            if (pin_class_debug) {
                printf("Pin.mapper maps ");
                mp_obj_print(user_obj, PRINT_REPR);
                printf(" to ");
                mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
                printf("\n");
            }
            return pin_obj;
        }
        // The pin mapping function returned mp_const_none, fall through to
        // other lookup methods.
    }

    if (pin_class_map_dict != MP_OBJ_NULL) {
        mp_map_t *pin_map_map = mp_obj_dict_get_map(pin_class_map_dict);
        mp_map_elem_t *elem = mp_map_lookup(pin_map_map, user_obj, MP_MAP_LOOKUP);
        if (elem != NULL && elem->value != NULL) {
            pin_obj = elem->value;
            if (pin_class_debug) {
                printf("Pin.map_dict maps ");
                mp_obj_print(user_obj, PRINT_REPR);
                printf(" to ");
                mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
                printf("\n");
            }
            return pin_obj;
        }
    }

    // See if the pin name matches a board pin
    const char *pin_name = mp_obj_str_get_str(user_obj);
    pin_obj = pin_find_named_pin(pin_board_pins, pin_name);
    if (pin_obj) {
        if (pin_class_debug) {
            printf("Pin.board maps ");
            mp_obj_print(user_obj, PRINT_REPR);
            printf(" to ");
            mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    // See if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(pin_cpu_pins, pin_name);
    if (pin_obj) {
        if (pin_class_debug) {
            printf("Pin.cpu maps ");
            mp_obj_print(user_obj, PRINT_REPR);
            printf(" to ");
            mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin '%s' not a valid pin identifier", pin_name));
}

STATIC void pin_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;
    print(env, "<Pin %s>", self->name);
}

// Pin constructor
STATIC mp_obj_t pin_make_new(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_check_nargs(n_args, 1, 3, n_kw, false);

    // Run an argument through the mapper and return the result.
    const pin_obj_t *pin = pin_find(args[0]);

    if (n_args >= 2) {
        // pin mode given, so configure this GPIO

        // get io mode
        uint mode = mp_obj_get_int(args[1]);
        if (!IS_GPIO_MODE(mode)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin mode: %d", mode));
        }

        // get pull mode
        uint pull = GPIO_NOPULL;
        if (n_args >= 3) {
            pull = mp_obj_get_int(args[2]);
            if (!IS_GPIO_PULL(pull)) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin pull: %d", pull));
            }
        }

        // configure the GPIO as requested
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.Pin = pin->pin_mask;
        GPIO_InitStructure.Mode = mode;
        GPIO_InitStructure.Pull = pull;
        GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
        GPIO_InitStructure.Alternate = 0;
        HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);
    }

    return (mp_obj_t)pin;
}

// class method
STATIC mp_obj_t pin_mapper(uint n_args, mp_obj_t *args) {
    if (n_args > 1) {
        pin_class_mapper = args[1];
        return mp_const_none;
    }
    return pin_class_mapper;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_mapper_fun_obj, 1, 2, pin_mapper);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_mapper_obj, (mp_obj_t)&pin_mapper_fun_obj);

// class method
STATIC mp_obj_t pin_map_dict(uint n_args, mp_obj_t *args) {
    if (n_args > 1) {
        pin_class_map_dict = args[1];
        return mp_const_none;
    }
    return pin_class_map_dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_dict_fun_obj, 1, 2, pin_map_dict);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_map_dict_obj, (mp_obj_t)&pin_map_dict_fun_obj);

// class method
STATIC mp_obj_t pin_debug(uint n_args, mp_obj_t *args) {
    if (n_args > 1) {
        pin_class_debug = mp_obj_is_true(args[1]);
        return mp_const_none;
    }
    return MP_BOOL(pin_class_debug);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_debug_fun_obj, 1, 2, pin_debug);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_debug_obj, (mp_obj_t)&pin_debug_fun_obj);

STATIC mp_obj_t pin_value(uint n_args, mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT((self->gpio->IDR >> self->port) & 1);
    } else {
        // set pin
        if (mp_obj_is_true(args[1])) {
            self->gpio->BSRRL = self->pin_mask;
        } else {
            self->gpio->BSRRH = self->pin_mask;
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

STATIC mp_obj_t pin_low(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    self->gpio->BSRRH = self->pin_mask;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_low_obj, pin_low);

STATIC mp_obj_t pin_high(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    self->gpio->BSRRL = self->pin_mask;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_high_obj, pin_high);

STATIC mp_obj_t pin_name(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_QSTR(qstr_from_str(self->name));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_name_obj, pin_name);

STATIC mp_obj_t pin_port(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)self->port);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_port_obj, pin_port);

STATIC mp_obj_t pin_pin(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)self->pin);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_pin_obj, pin_pin);

STATIC const mp_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),   (mp_obj_t)&pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),     (mp_obj_t)&pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),    (mp_obj_t)&pin_high_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_name),    (mp_obj_t)&pin_name_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_port),    (mp_obj_t)&pin_port_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin),     (mp_obj_t)&pin_pin_obj },

    // class methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_mapper),  (mp_obj_t)&pin_mapper_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dict),    (mp_obj_t)&pin_map_dict_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_debug),   (mp_obj_t)&pin_debug_obj },

    // class attributes
    { MP_OBJ_NEW_QSTR(MP_QSTR_board),   (mp_obj_t)&pin_board_pins_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cpu),     (mp_obj_t)&pin_cpu_pins_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_INPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT_PP),    MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_PP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT_OD),    MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_PP),     MP_OBJ_NEW_SMALL_INT(GPIO_MODE_AF_PP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_OD),     MP_OBJ_NEW_SMALL_INT(GPIO_MODE_AF_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ANALOG),    MP_OBJ_NEW_SMALL_INT(GPIO_MODE_ANALOG) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_NONE), MP_OBJ_NEW_SMALL_INT(GPIO_NOPULL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP),   MP_OBJ_NEW_SMALL_INT(GPIO_PULLUP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN), MP_OBJ_NEW_SMALL_INT(GPIO_PULLDOWN) },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};

STATIC void pin_af_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_af_obj_t *self = self_in;
    print(env, "<Pin AF %d fn:%d unit:%d typ:%d>", self->idx, self->fn,
          self->unit, self->type);
}

const mp_obj_type_t pin_af_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinAF,
    .print = pin_af_obj_print,
};
