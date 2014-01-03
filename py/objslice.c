#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"

#if MICROPY_ENABLE_SLICE

// TODO: This implements only variant of slice with 2 integer args only.
// CPython supports 3rd arg (step), plus args can be arbitrary Python objects.
typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    machine_int_t start;
    machine_int_t stop;
} mp_obj_slice_t;

void slice_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_slice_t *o = o_in;
    print(env, "slice(" INT_FMT ", " INT_FMT ")", o->start, o->stop);
}

const mp_obj_type_t slice_type = {
    { &mp_const_type },
    "slice",
    slice_print,
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    { { NULL, NULL }, }, // method list
};

// TODO: Make sure to handle "empty" values, which are signified by None in CPython
mp_obj_t mp_obj_new_slice(mp_obj_t ostart, mp_obj_t ostop, mp_obj_t ostep) {
    assert(ostep == NULL);
    machine_int_t start = mp_obj_get_int(ostart);
    machine_int_t stop = mp_obj_get_int(ostop);
    mp_obj_slice_t *o = m_new(mp_obj_slice_t, 1);
    o->base.type = &slice_type;
    o->start = start;
    o->stop = stop;
    return (mp_obj_t)o;
}

void mp_obj_slice_get(mp_obj_t self_in, machine_int_t *start, machine_int_t *stop, machine_int_t *step) {
    assert(MP_OBJ_IS_TYPE(self_in, &slice_type));
    mp_obj_slice_t *self = self_in;
    *start = self->start;
    *stop = self->stop;
    *step = 1;
}

#endif
