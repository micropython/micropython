// entire file can be wrapped in extern "C" {...} to access these functions from C files, but this isn't required. Their declarations would have to be moved to a C header.

#include <examplemodule.hpp>

typedef struct _cppclass_obj_t {
    mp_obj_base_t base;
    int16_t a;
    int16_t b;
} cppclass_obj_t;

extern const mp_obj_type_t cppclass_type; // extern keyword necessary, else linker undefined reference error

STATIC void cppclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    cppclass_obj_t *self = (cppclass_obj_t*)MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "cppclass(");
    mp_obj_print_helper(print, mp_obj_new_int(self->a), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(self->b), PRINT_REPR);
    mp_print_str(print, ")");
}

STATIC mp_obj_t cppclass_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, true);
    cppclass_obj_t *self = m_new_obj(cppclass_obj_t);
    self->base.type = &cppclass_type;
    self->a = mp_obj_get_int(args[0]);
    self->b = mp_obj_get_int(args[1]);
    return MP_OBJ_FROM_PTR(self);
}

// Class methods
STATIC mp_obj_t cppclass_sum(mp_obj_t self_in) {
    cppclass_obj_t *self = (cppclass_obj_t*)MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->a + self->b);
}

MP_DEFINE_CONST_FUN_OBJ_1(cppclass_sum_obj, cppclass_sum);

STATIC const mp_rom_map_elem_t cppclass_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_sum), MP_ROM_PTR(&cppclass_sum_obj) },
};

STATIC MP_DEFINE_CONST_DICT(cppclass_locals_dict, cppclass_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    cppclass_type,
    MP_TYPE_FLAG_NONE,
    MP_QSTR_cppclass,
    make_new, cppclass_make_new,
    print, cppclass_print,
    locals_dict, &cppclass_locals_dict
);

// Module functions
STATIC mp_obj_t cppusermod_func(const mp_obj_t o_in) {
    cppclass_obj_t *class_instance = (cppclass_obj_t*)MP_OBJ_TO_PTR(o_in);
    return mp_obj_new_int(class_instance->a + class_instance->b);
}

MP_DEFINE_CONST_FUN_OBJ_1(cppusermod_func_obj, cppusermod_func);

STATIC const mp_rom_map_elem_t cpp_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cppusermod) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cppclass), (mp_obj_t)&cppclass_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add), (mp_obj_t)&cppusermod_func_obj },
};

STATIC MP_DEFINE_CONST_DICT(cpp_module_globals,cpp_module_globals_table);

extern const mp_obj_module_t cpp_usermodule = { // extern keyword necessary, else linker undefined reference error
    .base = { &cpp_type_module },
    .globals = (mp_obj_dict_t*)&cpp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cppusermod, cpp_usermodule);
