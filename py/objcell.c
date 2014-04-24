#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_cell_t {
    mp_obj_base_t base;
    mp_obj_t obj;
} mp_obj_cell_t;

mp_obj_t mp_obj_cell_get(mp_obj_t self_in) {
    mp_obj_cell_t *self = self_in;
    return self->obj;
}

void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj) {
    mp_obj_cell_t *self = self_in;
    self->obj = obj;
}

#if 0
STATIC void cell_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_cell_t *o = o_in;
    print(env, "<cell ");
    if (o->obj == MP_OBJ_NULL) {
        print(env, "(nil)");
    } else {
        //print(env, "%p", o->obj);
        mp_obj_print_helper(print, env, o->obj, PRINT_REPR);
    }
    print(env, ">");
}
#endif

const mp_obj_type_t cell_type = {
    { &mp_type_type },
    .name = MP_QSTR_, // should never need to print cell type
    //.print = cell_print,
};

mp_obj_t mp_obj_new_cell(mp_obj_t obj) {
    mp_obj_cell_t *o = m_new_obj(mp_obj_cell_t);
    o->base.type = &cell_type;
    o->obj = obj;
    return o;
}
