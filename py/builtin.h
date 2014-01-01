// TODO convert all these to objects using MP_DECLARE and MP_DEFINE

mp_obj_t mp_builtin___build_class__(mp_obj_t o_class_fun, mp_obj_t o_class_name);
mp_obj_t mp_builtin___import__(int n, mp_obj_t *args);
mp_obj_t mp_builtin___repl_print__(mp_obj_t o);
mp_obj_t mp_builtin_abs(mp_obj_t o_in);
mp_obj_t mp_builtin_all(mp_obj_t o_in);
mp_obj_t mp_builtin_any(mp_obj_t o_in);
mp_obj_t mp_builtin_bool(int n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_callable(mp_obj_t o_in);
#if MICROPY_ENABLE_FLOAT
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_complex_obj);
#endif
mp_obj_t mp_builtin_chr(mp_obj_t o_in);
mp_obj_t mp_builtin_dict(void);
mp_obj_t mp_builtin_divmod(mp_obj_t o1_in, mp_obj_t o2_in);
#if MICROPY_ENABLE_FLOAT
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_float_obj);
#endif
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_hash_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_int_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_iter_obj);
mp_obj_t mp_builtin_len(mp_obj_t o_in);
mp_obj_t mp_builtin_list(int n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_max(int n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_min(int n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_next_obj);
mp_obj_t mp_builtin_ord(mp_obj_t o_in);
mp_obj_t mp_builtin_pow(int n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_print(int n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_range(int n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_set_obj);
mp_obj_t mp_builtin_sum(int n_args, const mp_obj_t *args);
