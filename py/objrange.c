#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

/******************************************************************************/
/* range iterator                                                             */

typedef struct _mp_obj_range_it_t {
    mp_obj_base_t base;
    // TODO make these values generic objects or something
    machine_int_t cur;
    machine_int_t stop;
    machine_int_t step;
} mp_obj_range_it_t;

STATIC mp_obj_t range_it_iternext(mp_obj_t o_in) {
    mp_obj_range_it_t *o = o_in;
    if ((o->step > 0 && o->cur < o->stop) || (o->step < 0 && o->cur > o->stop)) {
        mp_obj_t o_out = MP_OBJ_NEW_SMALL_INT(o->cur);
        o->cur += o->step;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t range_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = range_it_iternext,
};

mp_obj_t mp_obj_new_range_iterator(int cur, int stop, int step) {
    mp_obj_range_it_t *o = m_new_obj(mp_obj_range_it_t);
    o->base.type = &range_it_type;
    o->cur = cur;
    o->stop = stop;
    o->step = step;
    return o;
}

/******************************************************************************/
/* range                                                                      */

typedef struct _mp_obj_range_t {
    mp_obj_base_t base;
    // TODO make these values generic objects or something
    machine_int_t start;
    machine_int_t stop;
    machine_int_t step;
} mp_obj_range_t;

STATIC mp_obj_t range_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_check_nargs(n_args, 1, 3, n_kw, false);

    mp_obj_range_t *o = m_new_obj(mp_obj_range_t);
    o->base.type = &mp_type_range;
    o->start = 0;
    o->step = 1;

    if (n_args == 1) {
        o->stop = mp_obj_get_int(args[0]);
    } else {
        o->start = mp_obj_get_int(args[0]);
        o->stop = mp_obj_get_int(args[1]);
        if (n_args == 3) {
            o->step = mp_obj_get_int(args[2]);
        }
    }

    return o;
}

STATIC mp_obj_t range_getiter(mp_obj_t o_in) {
    mp_obj_range_t *o = o_in;
    return mp_obj_new_range_iterator(o->start, o->stop, o->step);
}

const mp_obj_type_t mp_type_range = {
    { &mp_type_type },
    .name = MP_QSTR_range,
    .make_new = range_make_new,
    .getiter = range_getiter,
};
