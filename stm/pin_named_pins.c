#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

#include "pin.h"

static void pin_named_pins_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_named_pins_obj_t *self = self_in;
    print(env, "<Pin.%s>", self->name);
}

static void pin_named_pins_obj_load_attr(mp_obj_t self_in, qstr attr_qstr, mp_obj_t *dest) {
    pin_named_pins_obj_t *self = self_in;
    const char *attr = qstr_str(attr_qstr);
    const pin_obj_t *pin = pin_find_named_pin(self->named_pins, attr);
    if (pin) {
        dest[0] = (mp_obj_t)pin;
        dest[1] = MP_OBJ_NULL;
    }
}

static const mp_obj_type_t pin_named_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinNamed,
    .print = pin_named_pins_obj_print,
    .load_attr = pin_named_pins_obj_load_attr,
};

const pin_named_pins_obj_t pin_board_pins_obj = {
    { &pin_named_pins_obj_type },
    .name = "board",
    .named_pins = pin_board_pins,
};

const pin_named_pins_obj_t pin_cpu_pins_obj = {
    { &pin_named_pins_obj_type },
    .name = "cpu",
    .named_pins = pin_cpu_pins,
};

const pin_obj_t *pin_find_named_pin(const pin_named_pin_t *named_pins, const char *name) {
    const pin_named_pin_t *named_pin = named_pins;
    while (named_pin->name) {
        if (!strcmp(name, named_pin->name)) {
            return named_pin->pin;
        }
        named_pin++;
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af(const pin_obj_t *pin, uint8_t fn, uint8_t unit, uint8_t type) {
    const pin_af_obj_t *af = pin->af;
    for (int i = 0; i < pin->num_af; i++, af++) {
        if (af->fn == fn && af->unit == unit && af->type == type) {
            return af;
        }
    }
    return NULL;
}
