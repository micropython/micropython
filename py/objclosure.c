#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "runtime.h"

typedef struct _mp_obj_closure_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    mp_obj_tuple_t *closure_tuple;
} mp_obj_closure_t;

mp_obj_t closure_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_closure_t *self = self_in;
    mp_obj_tuple_t *t = self->closure_tuple;

    // need to concatenate closed-over-vars and args

    int n_total = t->len + n_args + 2 * n_kw;
    if (n_total <= 5) {
        // use stack to allocate temporary args array
        mp_obj_t args2[5];
        memcpy(args2, t->items, t->len * sizeof(mp_obj_t));
        memcpy(args2 + t->len, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
        return mp_call_function_n_kw(self->fun, t->len + n_args, n_kw, args2);
    } else {
        // use heap to allocate temporary args array
        mp_obj_t *args2 = m_new(mp_obj_t, n_total);
        memcpy(args2, t->items, t->len * sizeof(mp_obj_t));
        memcpy(args2 + t->len, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
        mp_obj_t res = mp_call_function_n_kw(self->fun, t->len + n_args, n_kw, args2);
        m_del(mp_obj_t, args2, n_total);
        return res;
    }
}

#if 0
STATIC void closure_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_closure_t *o = o_in;
    print(env, "<closure %p (closed: %p) ", o, o->closure_tuple);
    mp_obj_tuple_t *t = o->closure_tuple;
    for (int i = 0; i < t->len; i++) {
        if (t->items[i] == MP_OBJ_NULL) {
            print(env, "(nil)");
        } else {
            mp_obj_print_helper(print, env, t->items[i], PRINT_REPR);
        }
        print(env, " ");
    }
    print(env, ">");
}
#endif

const mp_obj_type_t closure_type = {
    { &mp_type_type },
    .name = MP_QSTR_closure,
    //.print = closure_print,
    .call = closure_call,
};

mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple) {
    mp_obj_closure_t *o = m_new_obj(mp_obj_closure_t);
    o->base.type = &closure_type;
    o->fun = fun;
    o->closure_tuple = closure_tuple;
    return o;
}
