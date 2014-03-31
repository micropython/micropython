#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"

#include "pin.h"

void pin_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;
    print(env, "<Pin %s>", self->name);
}

mp_obj_t pin_obj_name(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_QSTR(qstr_from_str(self->name));
}

mp_obj_t pin_obj_port(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)self->port);
}

mp_obj_t pin_obj_pin(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)self->pin);
}

static MP_DEFINE_CONST_FUN_OBJ_1(pin_obj_name_obj, pin_obj_name);
static MP_DEFINE_CONST_FUN_OBJ_1(pin_obj_port_obj, pin_obj_port);
static MP_DEFINE_CONST_FUN_OBJ_1(pin_obj_pin_obj, pin_obj_pin);

STATIC const mp_map_elem_t pin_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_name), (mp_obj_t)&pin_obj_name_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_port), (mp_obj_t)&pin_obj_port_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin), (mp_obj_t)&pin_obj_pin_obj },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_obj_print,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};

void pin_af_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_af_obj_t *self = self_in;
    print(env, "<Pin AF %d fn:%d unit:%d typ:%d>", self->idx, self->fn,
          self->unit, self->type);
}

const mp_obj_type_t pin_af_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinAF,
    .print = pin_af_obj_print,
};

