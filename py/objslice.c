#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"

/******************************************************************************/
/* ellipsis object, a singleton                                               */

typedef struct _mp_obj_ellipsis_t {
    mp_obj_base_t base;
} mp_obj_ellipsis_t;

void ellipsis_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "Ellipsis");
}

const mp_obj_type_t mp_type_ellipsis = {
    { &mp_type_type },
    .name = MP_QSTR_Ellipsis,
    .print = ellipsis_print,
};

const mp_obj_ellipsis_t mp_const_ellipsis_obj = {{&mp_type_ellipsis}};

/******************************************************************************/
/* slice object                                                               */

#if MICROPY_ENABLE_SLICE

// TODO: This implements only variant of slice with 2 integer args only.
// CPython supports 3rd arg (step), plus args can be arbitrary Python objects.
typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    machine_int_t start;
    machine_int_t stop;
} mp_obj_slice_t;

void slice_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_slice_t *o = o_in;
    print(env, "slice(" INT_FMT ", " INT_FMT ")", o->start, o->stop);
}

const mp_obj_type_t mp_type_slice = {
    { &mp_type_type },
    .name = MP_QSTR_slice,
    .print = slice_print,
};

// TODO: Make sure to handle "empty" values, which are signified by None in CPython
mp_obj_t mp_obj_new_slice(mp_obj_t ostart, mp_obj_t ostop, mp_obj_t ostep) {
    assert(ostep == NULL);
    machine_int_t start = 0, stop = 0;
    if (ostart != mp_const_none) {
        start = mp_obj_get_int(ostart);
    }
    if (ostop != mp_const_none) {
        stop = mp_obj_get_int(ostop);
        if (stop == 0) {
            // [x:0] is a special case - in our slice object, stop = 0 means
            // "end of sequence". Fortunately, [x:0] is an empty seqence for
            // any x (including negative). [x:x] is also always empty sequence.
            // but x also can be 0. But note that b""[x:x] is b"" for any x (i.e.
            // no IndexError, at least in Python 3.3.3). So, we just use -1's to
            // signify that. -1 is catchy "special" number in case someone will
            // try to print [x:0] slice ever.
            start = stop = -1;
        }
    }
    mp_obj_slice_t *o = m_new(mp_obj_slice_t, 1);
    o->base.type = &mp_type_slice;
    o->start = start;
    o->stop = stop;
    return (mp_obj_t)o;
}

void mp_obj_slice_get(mp_obj_t self_in, machine_int_t *start, machine_int_t *stop, machine_int_t *step) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_slice));
    mp_obj_slice_t *self = self_in;
    *start = self->start;
    *stop = self->stop;
    *step = 1;
}

#endif
