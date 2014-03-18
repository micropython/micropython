#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "nlr.h"
#include "map.h"

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
// followed by the pin number. On the PYBOARD4, pyb.Pin.board.X1 and
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

static void pin_map_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    print(env, "<PinMap>");
}

static mp_obj_t pin_map_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    pin_map_obj_t *self = self_in;
    rt_check_nargs(n_args, 1, 2, n_kw, false);

    if (n_args > 1) {
        if (!self->map_dict) {
            self->map_dict = mp_obj_new_dict(1);
        }
        mp_obj_dict_store(self->map_dict, args[0], args[1]);
        return mp_const_none;
    }

    // Run an argument through the mapper and return the result.
    return (mp_obj_t)pin_map_user_obj(args[0]);
}

static mp_obj_t pin_map_obj_mapper(uint n_args, mp_obj_t *args) {
    pin_map_obj_t *self = args[0];
    if (n_args > 1) {
        self->mapper = args[1];
        return mp_const_none;
    }
    return self->mapper;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_obj_mapper_obj, 1, 2, pin_map_obj_mapper);

static mp_obj_t pin_map_obj_debug(uint n_args, mp_obj_t *args) {
    pin_map_obj_t *self = args[0];
    if (n_args > 1) {
        self->debug = rt_is_true(args[1]);
        return mp_const_none;
    }
    return MP_BOOL(self->debug);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_obj_debug_obj, 1, 2, pin_map_obj_debug);

static void pin_map_load_attr(mp_obj_t self_in, qstr attr_qstr, mp_obj_t *dest) {
    (void)self_in;
    const char *attr = qstr_str(attr_qstr);

    if (strcmp(attr, "mapper") == 0) {
        dest[0] = (mp_obj_t)&pin_map_obj_mapper_obj;
        dest[1] = self_in;
    }
    if (strcmp(attr, "debug") == 0) {
        dest[0] = (mp_obj_t)&pin_map_obj_debug_obj;
        dest[1] = self_in;
    }
    if (strcmp(attr, pin_board_pins_obj.name) == 0) {
        dest[0] = (mp_obj_t)&pin_board_pins_obj;
        dest[1] = MP_OBJ_NULL;
    }
    if (strcmp(attr, pin_cpu_pins_obj.name) == 0) {
        dest[0] = (mp_obj_t)&pin_cpu_pins_obj;
        dest[1] = MP_OBJ_NULL;
    }
}

static const mp_obj_type_t pin_map_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinMap,
    .print = pin_map_obj_print,
    .call = pin_map_call,
    .load_attr = pin_map_load_attr,
};

static const pin_map_obj_t pin_map_obj_init = {
    { &pin_map_obj_type },
    .mapper = MP_OBJ_NULL,
    .map_dict = MP_OBJ_NULL,
    .debug = false,
};

pin_map_obj_t pin_map_obj;

void pin_map_init(void) {
    pin_map_obj = pin_map_obj_init;
}

// C API used to convert a user-supplied pin name  into an ordinal pin number.
const pin_obj_t *pin_map_user_obj(mp_obj_t user_obj) {
    const pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (MP_OBJ_IS_TYPE(user_obj, &pin_obj_type)) {
        pin_obj = user_obj;
        if (pin_map_obj.debug) {
            printf("Pin map passed pin ");
            mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    if (pin_map_obj.mapper) {
        pin_obj = rt_call_function_1(pin_map_obj.mapper, user_obj);
        if (pin_obj != mp_const_none) {
            if (!MP_OBJ_IS_TYPE(pin_obj, &pin_obj_type)) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "Pin.mapper didn't return a Pin object"));
            }
            if (pin_map_obj.debug) {
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

    if (pin_map_obj.map_dict) {
        mp_map_t *pin_map_map = mp_obj_dict_get_map(pin_map_obj.map_dict);
        mp_map_elem_t *elem = mp_map_lookup(pin_map_map, user_obj, MP_MAP_LOOKUP);
        if (elem != NULL && elem->value != NULL) {
            pin_obj = elem->value;
            if (pin_map_obj.debug) {
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
        if (pin_map_obj.debug) {
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
        if (pin_map_obj.debug) {
            printf("Pin.cpu maps ");
            mp_obj_print(user_obj, PRINT_REPR);
            printf(" to ");
            mp_obj_print((mp_obj_t)pin_obj, PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin '%s' not a valid pin identifier", pin_name));
}
