#define MICROPY_ENABLE_DYNRUNTIME (1)
#define MICROPY_PY_UZLIB (1)

#include "py/dynruntime.h"

#if !defined(__linux__)
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}
#endif

mp_obj_type_t decompio_type;

#include "extmod/moduzlib.c"

mp_map_elem_t decompio_locals_dict_table[3];
STATIC MP_DEFINE_CONST_DICT(decompio_locals_dict, decompio_locals_dict_table);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    decompio_type.base.type = mp_fun_table.type_type;
    decompio_type.name = MP_QSTR_DecompIO;
    decompio_type.make_new = decompio_make_new;
    decompio_type.protocol = &decompio_stream_p;
    decompio_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_read), MP_OBJ_FROM_PTR(&mp_stream_read_obj) };
    decompio_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_OBJ_FROM_PTR(&mp_stream_readinto_obj) };
    decompio_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_readline), MP_OBJ_FROM_PTR(&mp_stream_unbuffered_readline_obj) };
    decompio_type.locals_dict = (void*)&decompio_locals_dict;

    mp_store_global(MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR_uzlib));
    mp_store_global(MP_QSTR_decompress, MP_OBJ_FROM_PTR(&mod_uzlib_decompress_obj));
    mp_store_global(MP_QSTR_DecompIO, MP_OBJ_FROM_PTR(&decompio_type));

    MP_DYNRUNTIME_INIT_EXIT
}
