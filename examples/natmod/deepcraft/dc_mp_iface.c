#include "py/dynruntime.h"

#if !defined(__linux__)
void *memcpy(void *dst, const void *src, size_t n) {
    return mp_fun_table.memmove_(dst, src, n);
}
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}
#endif

int native_errno=0;
#if defined(__linux__)
int *__errno_location (void)
#else
int *__errno (void)
#endif
{
    return &native_errno;
}

#include "examples/natmod/deepcraft/mp_src.c"

// Forward declaration of type
mp_obj_full_type_t dc_type;

mp_map_elem_t dc_locals_dict_table[5];
static MP_DEFINE_CONST_DICT(dc_locals_dict, dc_locals_dict_table);

// Constructor
static mp_obj_t dc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    dc_obj_t *self = mp_obj_malloc(dc_obj_t, type);
    return MP_OBJ_FROM_PTR(self);
}

// Create type and methods at runtime
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Populate type
    dc_type.base.type = (void*)&mp_type_type;
    dc_type.flags = MP_TYPE_FLAG_NONE;
    dc_type.name = MP_QSTR_DEEPCRAFT;
    MP_OBJ_TYPE_SET_SLOT(&dc_type, make_new, dc_make_new, 0);

    dc_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_OBJ_FROM_PTR(&init_obj) };
    dc_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_enqueue), MP_OBJ_FROM_PTR(&enqueue_obj) };
    dc_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_dequeue), MP_OBJ_FROM_PTR(&dequeue_obj) };
    dc_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_get_model_input_dim), MP_OBJ_FROM_PTR(&get_model_input_dim_obj) };
    dc_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_get_model_output_dim), MP_OBJ_FROM_PTR(&get_model_output_dim_obj) };
    
    MP_OBJ_TYPE_SET_SLOT(&dc_type, locals_dict, (void*)&dc_locals_dict, 5);

    // Expose constructor as DEEPCRAFT
    mp_store_global(MP_QSTR_DEEPCRAFT, MP_OBJ_FROM_PTR(&dc_type));
    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
    
}
