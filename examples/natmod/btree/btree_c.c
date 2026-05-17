#define MICROPY_PY_BTREE (1)

#include "py/dynruntime.h"

#include <unistd.h>

#if !defined(__linux__)
void *memcpy(void *dst, const void *src, size_t n) {
    return mp_fun_table.memmove_(dst, src, n);
}
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}
#endif

void *memmove(void *dest, const void *src, size_t n) {
    return mp_fun_table.memmove_(dest, src, n);
}

void *malloc(size_t n) {
    void *ptr = m_malloc(n);
    return ptr;
}
void *realloc(void *ptr, size_t n) {
    mp_printf(&mp_plat_print, "UNDEF %d\n", __LINE__);
    return NULL;
}
void *calloc(size_t n, size_t m) {
    void *ptr = m_malloc(n * m);
    // memory already cleared by conservative GC
    return ptr;
}

void free(void *ptr) {
    m_free(ptr);
}

void abort_(void) {
    nlr_raise(mp_obj_new_exception(mp_load_global(MP_QSTR_RuntimeError)));
}

int puts(const char *s) {
    return mp_printf(&mp_plat_print, "%s\n", s);
}

int native_errno;
#if defined(__linux__)
int *__errno_location (void)
#else
int *__errno (void)
#endif
{
    return &native_errno;
}

ssize_t mp_stream_posix_write(void *stream, const void *buf, size_t len) {
    mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t out_sz = stream_p->write(MP_OBJ_FROM_PTR(stream), buf, len, &native_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

ssize_t mp_stream_posix_read(void *stream, void *buf, size_t len) {
    mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t out_sz = stream_p->read(MP_OBJ_FROM_PTR(stream), buf, len, &native_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

off_t mp_stream_posix_lseek(void *stream, off_t offset, int whence) {
    const mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    struct mp_stream_seek_t seek_s;
    seek_s.offset = offset;
    seek_s.whence = whence;
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &native_errno);
    if (res == MP_STREAM_ERROR) {
        return -1;
    }
    return seek_s.offset;
}

int mp_stream_posix_fsync(void *stream) {
    mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_FLUSH, 0, &native_errno);
    if (res == MP_STREAM_ERROR) {
        return -1;
    }
    return res;
}

mp_obj_full_type_t btree_type;
mp_getiter_iternext_custom_t btree_getiter_iternext;

#include "extmod/modbtree.c"

mp_map_elem_t btree_locals_dict_table[8];
static MP_DEFINE_CONST_DICT(btree_locals_dict, btree_locals_dict_table);

static mp_obj_t btree_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // The allowed_args array must have its qstr's populated at runtime.
    enum { ARG_flags, ARG_cachesize, ARG_pagesize, ARG_minkeypage };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    allowed_args[0].qst = MP_QSTR_flags;
    allowed_args[1].qst = MP_QSTR_cachesize;
    allowed_args[2].qst = MP_QSTR_pagesize;
    allowed_args[3].qst = MP_QSTR_minkeypage;

    // Make sure we got a stream object
    mp_get_stream_raise(pos_args[0], MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    BTREEINFO openinfo = {0};
    openinfo.flags = args[ARG_flags].u_int;
    openinfo.cachesize = args[ARG_cachesize].u_int;
    openinfo.psize = args[ARG_pagesize].u_int;
    openinfo.minkeypage = args[ARG_minkeypage].u_int;
    DB *db = __bt_open(MP_OBJ_TO_PTR(pos_args[0]), &btree_stream_fvtable, &openinfo, 0);
    if (db == NULL) {
        mp_raise_OSError(native_errno);
    }

    return MP_OBJ_FROM_PTR(btree_new(db, pos_args[0]));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(btree_open_obj, 1, btree_open);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    btree_getiter_iternext.getiter = btree_getiter;
    btree_getiter_iternext.iternext = btree_iternext;

    btree_type.base.type = (void*)&mp_fun_table.type_type;
    btree_type.flags = MP_TYPE_FLAG_ITER_IS_CUSTOM;
    btree_type.name = MP_QSTR_btree;
    MP_OBJ_TYPE_SET_SLOT(&btree_type, print, btree_print, 0);
    MP_OBJ_TYPE_SET_SLOT(&btree_type, iter, &btree_getiter_iternext, 1);
    MP_OBJ_TYPE_SET_SLOT(&btree_type, binary_op, btree_binary_op, 2);
    MP_OBJ_TYPE_SET_SLOT(&btree_type, subscr, btree_subscr, 3);
    btree_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_OBJ_FROM_PTR(&btree_close_obj) };
    btree_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_flush), MP_OBJ_FROM_PTR(&btree_flush_obj) };
    btree_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_get), MP_OBJ_FROM_PTR(&btree_get_obj) };
    btree_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_put), MP_OBJ_FROM_PTR(&btree_put_obj) };
    btree_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_seq), MP_OBJ_FROM_PTR(&btree_seq_obj) };
    btree_locals_dict_table[5] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_keys), MP_OBJ_FROM_PTR(&btree_keys_obj) };
    btree_locals_dict_table[6] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_values), MP_OBJ_FROM_PTR(&btree_values_obj) };
    btree_locals_dict_table[7] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_items), MP_OBJ_FROM_PTR(&btree_items_obj) };
    MP_OBJ_TYPE_SET_SLOT(&btree_type, locals_dict, (void*)&btree_locals_dict, 4);

    mp_store_global(MP_QSTR_open, MP_OBJ_FROM_PTR(&btree_open_obj));
    mp_store_global(MP_QSTR_INCL, MP_OBJ_NEW_SMALL_INT(FLAG_END_KEY_INCL));
    mp_store_global(MP_QSTR_DESC, MP_OBJ_NEW_SMALL_INT(FLAG_DESC));

    MP_DYNRUNTIME_INIT_EXIT
}
