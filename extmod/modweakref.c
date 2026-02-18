
#include <stdio.h>
#include <string.h>

#include "py/runtime.h"

#include "extmod/modweakref.h"

#if MICROPY_PY_WEAKREF

static const mp_obj_type_t mp_type_weakref_ref;

struct _mp_obj_weakref_ref_t {
    mp_obj_base_t base;
    volatile mp_weak_obj_t weakobj;
};

mp_obj_t weakref_ref_new(mp_obj_t obj) {
    mp_obj_weakref_ref_t *o = mp_obj_malloc(mp_obj_weakref_ref_t, &mp_type_weakref_ref);
    o->weakobj = MP_OBJ_TO_WEAK(obj);
    return MP_OBJ_FROM_PTR(o);
}

void weakref_ref_set(mp_obj_weakref_ref_t *self, mp_obj_t obj) {
    self->weakobj = MP_OBJ_TO_WEAK(obj);
}

mp_obj_t weakref_ref_get(const mp_obj_weakref_ref_t *self) {
    volatile mp_obj_t obj; // MUST be visible to GC tracing for correctness
    mp_weak_obj_t weakobj, weakobj_next = self->weakobj;
    do {
        weakobj = weakobj_next;
        obj = MP_OBJ_FROM_WEAK(weakobj);
        weakobj_next = self->weakobj;
    } while (weakobj != weakobj_next);
    return obj;
}


static mp_obj_t weakref_ref_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    return weakref_ref_new(args[0]);
}

static mp_obj_t weakref_ref_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_weakref_ref_t *self = MP_OBJ_TO_PTR(self_in);

    if (n_args == 1) { // update the weakref to point to a new object
        weakref_ref_set(self, args[0]);
        return mp_const_none;
    } else { // n_args == 0, dereference the weakref
        return weakref_ref_get(self);
    }
}

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_weakref_ref,
    MP_QSTR_ref,
    MP_TYPE_FLAG_NONE,
    make_new, weakref_ref_make_new,
    call, weakref_ref_call
    );

static const mp_rom_map_elem_t mp_module_weakref_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__weakref) },
    { MP_ROM_QSTR(MP_QSTR_ref), MP_ROM_PTR(&mp_type_weakref_ref) },
};
static MP_DEFINE_CONST_DICT(mp_module_weakref_globals, mp_module_weakref_globals_table);

const mp_obj_module_t mp_module_weakref = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_weakref_globals,
};

MP_REGISTER_MODULE(MP_QSTR__weakref, mp_module_weakref);

#endif // MICROPY_PY_WEAKREF
