


#ifndef MICROPY_INCLUDED_PY_MODBUILTINS_H
#define MICROPY_INCLUDED_PY_MODBUILTINS_H

#if ZVM_EXTMOD
extern mp_obj_t mp_builtin_bin(mp_obj_t o_in);

extern const mp_obj_type_t mp_builtin_zdict_type;

//zdict
typedef struct _mp_obj_zdict_t {
    mp_obj_base_t base;
    const char* storage_key;
    size_t storage_key_len;
} mp_obj_zdict_t;

typedef struct _mp_obj_register_t {
    mp_obj_base_t base;
    mp_obj_list_t* public_funcs;
} mp_obj_register_t;

typedef struct _mp_obj_decorator_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
    uint32_t params_data;
    const char* func;
} mp_obj_decorator_fun_t;

extern const char DICT_FORMAT_C;
extern const char STR_FORMAT_C;
extern const char INT_FORMAT_C;
extern const char SMALLINT_FORMAT_C;
extern const char LIST_FORMAT_C;
extern const char BOOL_FORMAT_C;
extern const char NONE_FORMAT_C;
extern const char BYTE_FORMAT_C;

extern void mp_obj_storage_value(const mp_obj_t value, byte** storage_value, size_t* storage_value_len);
extern mp_obj_t mp_obj_new_storage_value(mp_obj_t self_in, const char* storage_key, size_t storage_key_len);

#endif //ZVM_EXTMOD
#endif //MICROPY_INCLUDED_PY_MODBUILTINS_H
