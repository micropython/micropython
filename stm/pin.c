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

static const mp_method_t pin_methods[] = {
    { "name", &pin_obj_name_obj },
    { "port", &pin_obj_port_obj },
    { "pin", &pin_obj_pin_obj },
    { NULL, NULL },
};

const mp_obj_type_t pin_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_obj_print,
    .methods = pin_methods,
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

