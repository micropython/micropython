// Auto-generated module init function
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR_double, MP_OBJ_FROM_PTR(&double_value_obj));
    mp_store_global(MP_QSTR_add, MP_OBJ_FROM_PTR(&add_values_obj));
    mp_store_global(MP_QSTR_message, MP_OBJ_FROM_PTR(&get_message_obj));
    mp_store_global(MP_QSTR_MAGIC_NUMBER, MP_OBJ_NEW_SMALL_INT(42));

    MP_DYNRUNTIME_INIT_EXIT
}