#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"

/******************************************************************************/
/* range                                                                      */

typedef struct _mp_obj_range_t {
    mp_obj_base_t base;
    // TODO make these values generic objects or something
    machine_int_t start;
    machine_int_t stop;
    machine_int_t step;
} mp_obj_range_t;

mp_obj_t range_getiter(mp_obj_t o_in) {
    mp_obj_range_t *o = o_in;
    return mp_obj_new_range_iterator(o->start, o->stop, o->step);
}

static const mp_obj_type_t range_type = {
    .base = { &mp_const_type} ,
    .name = "range",
    .getiter = range_getiter,
    .methods = {{NULL, NULL},},
};

// range is a class and instances are immutable sequence objects
mp_obj_t mp_obj_new_range(int start, int stop, int step) {
    mp_obj_range_t *o = m_new_obj(mp_obj_range_t);
    o->base.type = &range_type;
    o->start = start;
    o->stop = stop;
    o->step = step;
    return o;
}

/******************************************************************************/
/* range iterator                                                             */

typedef struct _mp_obj_range_it_t {
    mp_obj_base_t base;
    // TODO make these values generic objects or something
    machine_int_t cur;
    machine_int_t stop;
    machine_int_t step;
} mp_obj_range_it_t;

mp_obj_t range_it_iternext(mp_obj_t o_in) {
    mp_obj_range_it_t *o = o_in;
    if ((o->step > 0 && o->cur < o->stop) || (o->step < 0 && o->cur > o->stop)) {
        mp_obj_t o_out = MP_OBJ_NEW_SMALL_INT(o->cur);
        o->cur += o->step;
        return o_out;
    } else {
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t range_it_type = {
    .base = { &mp_const_type },
    .name = "range_iterator",
    .iternext = range_it_iternext,
    .methods = {{NULL, NULL},},
};

mp_obj_t mp_obj_new_range_iterator(int cur, int stop, int step) {
    mp_obj_range_it_t *o = m_new_obj(mp_obj_range_it_t);
    o->base.type = &range_it_type;
    o->cur = cur;
    o->stop = stop;
    o->step = step;
    return o;
}
