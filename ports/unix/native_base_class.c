#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"

#if defined(MICROPY_UNIX_COVERAGE)

// This is a native base class that is used in tests.

typedef struct {
    mp_obj_base_t base;
    mp_obj_t test;
} native_base_class_obj_t;

const mp_obj_type_t native_base_class_type;

STATIC mp_obj_t native_base_class_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_test };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_test, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    native_base_class_obj_t *self = mp_obj_malloc(native_base_class_obj_t, &native_base_class_type);
    self->test = args[ARG_test].u_obj;
    return MP_OBJ_FROM_PTR(self);
}

// Helper to ensure we have the native super class instead of a subclass.
static native_base_class_obj_t *native_base(mp_obj_t unknown_obj) {
    mp_obj_t native_obj = mp_obj_cast_to_native_base(unknown_obj, &native_base_class_type);
    mp_obj_assert_native_inited(native_obj);
    return MP_OBJ_TO_PTR(native_obj);
}

STATIC mp_obj_t native_base_class_obj_get_test(mp_obj_t self_in) {
    native_base_class_obj_t *self = native_base(self_in);
    return self->test;
}
MP_DEFINE_CONST_FUN_OBJ_1(native_base_class_get_test_obj, native_base_class_obj_get_test);

STATIC mp_obj_t native_base_class_obj_set_test(mp_obj_t self_in, mp_obj_t value) {
    mp_printf(&mp_plat_print, "native base class .test set to: ");
    mp_obj_print_helper(&mp_plat_print, value, PRINT_REPR);
    mp_printf(&mp_plat_print, "\n");
    native_base_class_obj_t *self = native_base(self_in);
    self->test = value;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(native_base_class_set_test_obj, native_base_class_obj_set_test);

MP_PROPERTY_GETSET(native_base_class_test_obj,
    (mp_obj_t)&native_base_class_get_test_obj,
    (mp_obj_t)&native_base_class_set_test_obj);

STATIC const mp_rom_map_elem_t native_base_class_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_test), MP_ROM_PTR(&native_base_class_test_obj) },
};
STATIC MP_DEFINE_CONST_DICT(native_base_class_locals_dict, native_base_class_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    native_base_class_type,
    MP_QSTR_NativeBaseClass,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, &native_base_class_make_new,
    locals_dict, &native_base_class_locals_dict
    );

#endif
