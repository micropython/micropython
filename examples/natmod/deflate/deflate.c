#define MICROPY_PY_DEFLATE (1)
#define MICROPY_PY_DEFLATE_COMPRESS (1)

#include "py/dynruntime.h"

#if !defined(__linux__)
void *memcpy(void *dst, const void *src, size_t n) {
    return mp_fun_table.memmove_(dst, src, n);
}
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}
#endif

mp_obj_full_type_t deflateio_type;

#include "extmod/moddeflate.c"

// Re-implemented from py/stream.c, not yet available in dynruntime.h.
mp_obj_t mp_stream_close(mp_obj_t stream) {
    const mp_stream_p_t *stream_p = mp_get_stream(stream);
    int error;
    mp_uint_t res = stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_close_obj, mp_stream_close);

// Re-implemented from py/stream.c, not yet available in dynruntime.h.
static mp_obj_t mp_stream___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream___exit___obj, 4, 4, mp_stream___exit__);

// Re-implemented from obj.c, not yet available in dynruntime.h.
mp_obj_t mp_identity(mp_obj_t self) {
    return self;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_identity_obj, mp_identity);

mp_map_elem_t deflateio_locals_dict_table[7];
static MP_DEFINE_CONST_DICT(deflateio_locals_dict, deflateio_locals_dict_table);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    deflateio_type.base.type = mp_fun_table.type_type;
    deflateio_type.name = MP_QSTR_DeflateIO;
    MP_OBJ_TYPE_SET_SLOT(&deflateio_type, make_new, &deflateio_make_new, 0);
    MP_OBJ_TYPE_SET_SLOT(&deflateio_type, protocol, &deflateio_stream_p, 1);
    deflateio_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_read), MP_OBJ_FROM_PTR(&mp_stream_read_obj) };
    deflateio_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_OBJ_FROM_PTR(&mp_stream_readinto_obj) };
    deflateio_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_readline), MP_OBJ_FROM_PTR(&mp_stream_unbuffered_readline_obj) };
    deflateio_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_write), MP_OBJ_FROM_PTR(&mp_stream_write_obj) };
    deflateio_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_OBJ_FROM_PTR(&mp_stream_close_obj) };
    deflateio_locals_dict_table[5] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR___enter__), MP_OBJ_FROM_PTR(&mp_identity_obj) };
    deflateio_locals_dict_table[6] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR___exit__), MP_OBJ_FROM_PTR(&mp_stream___exit___obj) };
    MP_OBJ_TYPE_SET_SLOT(&deflateio_type, locals_dict, (void*)&deflateio_locals_dict, 2);

    mp_store_global(MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR_deflate));
    mp_store_global(MP_QSTR_DeflateIO, MP_OBJ_FROM_PTR(&deflateio_type));
    mp_store_global(MP_QSTR_RAW, MP_OBJ_NEW_SMALL_INT(DEFLATEIO_FORMAT_RAW));
    mp_store_global(MP_QSTR_ZLIB, MP_OBJ_NEW_SMALL_INT(DEFLATEIO_FORMAT_ZLIB));
    mp_store_global(MP_QSTR_GZIP, MP_OBJ_NEW_SMALL_INT(DEFLATEIO_FORMAT_GZIP));

    MP_DYNRUNTIME_INIT_EXIT
}
