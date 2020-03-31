#include <py/obj.h>
#include <py/runtime.h>

//Convert const char* -> qstr -> mp_objt_t.
mp_obj_t new_qstr_obj(const char *str) {
    return MP_OBJ_NEW_QSTR(qstr_from_str(str));
}

//Example function which adds 1 to the integer passed.
mp_obj_t add_one(mp_obj_t arg) {
    return mp_obj_new_int(mp_obj_get_int(arg) + 1);
}
MP_DEFINE_CONST_FUN_OBJ_1(add_one_obj, add_one);

//Example class which wraps an integer.
typedef struct _mp_obj_number_t {
    mp_obj_base_t base;
    int number;
} mp_obj_number_t;

mp_obj_t number_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_number_t *o = m_new_obj(mp_obj_number_t);
    o->base.type = type;
    o->number = mp_obj_get_int(args[0]);
    return MP_OBJ_FROM_PTR(o);
}

void number_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_number_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "number(%d)", self->number);
}

mp_obj_t number_add(mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_number_t *self = MP_OBJ_TO_PTR(lhs);
    mp_obj_number_t *rhs_in = MP_OBJ_TO_PTR(rhs);
    if (!mp_obj_is_type(rhs_in, self->base.type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("right-hand side must be a <number>"));
    }
    self->number += rhs_in->number;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(number_add_obj, number_add);

//Module initialization funtion called upon 'import dynmod'.
mp_obj_module_t* init_dynmod() {
    mp_obj_module_t *mod = (mp_obj_module_t*) mp_obj_new_module(qstr_from_str("dynmod"));
    //Add the function.
    mp_obj_dict_store(MP_OBJ_FROM_PTR(mod->globals), new_qstr_obj("add_one"), MP_OBJ_FROM_PTR(&add_one_obj));
    //Create clas definition and add it.
    mp_obj_type_t *number_type = m_new0(mp_obj_type_t, 1);
    number_type->base.type = &mp_type_type;
    number_type->name = qstr_from_str("number");
    number_type->make_new = number_make_new;
    number_type->print = number_print;
    number_type->locals_dict = mp_obj_new_dict(0);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(number_type->locals_dict), new_qstr_obj("add"), MP_OBJ_FROM_PTR(&number_add_obj));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(mod->globals), new_qstr_obj("number"), MP_OBJ_FROM_PTR(number_type));
    return mod;
}
