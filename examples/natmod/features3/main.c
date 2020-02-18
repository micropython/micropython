// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

mp_obj_type_t native_type;

typedef struct _mp_obj_native_t {
    mp_obj_base_t base;
    mp_int_t value;
} mp_obj_native_t;

mp_map_elem_t native_locals_dict_table[3];
STATIC MP_DEFINE_CONST_DICT(native_locals_dict, native_locals_dict_table);

STATIC mp_obj_t native_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_native_t *self = m_new_obj(mp_obj_native_t);
    self->base.type = type;
    self->value = 0;
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t call_foo(mp_obj_t self_in) {
    mp_obj_native_t *self = mp_obj_cast_to_native_base_unchecked(self_in);

    // Call self.foo(value) which should go to the derived object
    mp_obj_t dest[3];
    mp_fun_table.load_method(self_in, MP_QSTR_foo, dest);
    dest[2] = mp_obj_new_int(self->value);
    mp_fun_table.call_method_n_kw(1, 0, dest);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(call_foo_obj, call_foo);

STATIC mp_obj_t add(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_native_t *self = mp_obj_cast_to_native_base_unchecked(self_in);
    self->value += mp_obj_get_int(arg);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(add_obj, add);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    native_type.base.type = &mp_type_type;
    native_type.flags = MP_TYPE_FLAG_CAN_HANDLE_SUBCLASS;
    native_type.name = MP_QSTR_Native;
    native_type.make_new = native_make_new;
    native_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_add), MP_OBJ_FROM_PTR(&add_obj) };
    native_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_call_foo), MP_OBJ_FROM_PTR(&call_foo_obj) };
    native_type.locals_dict = (void*)&native_locals_dict;

    mp_store_global(MP_QSTR_Native, MP_OBJ_FROM_PTR(&native_type));

    MP_DYNRUNTIME_INIT_EXIT
}
