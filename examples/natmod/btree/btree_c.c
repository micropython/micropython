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
    const mp_stream_p_t *stream_p = o->type->protocol;
    mp_uint_t out_sz = stream_p->write(MP_OBJ_FROM_PTR(stream), buf, len, &native_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

ssize_t mp_stream_posix_read(void *stream, void *buf, size_t len) {
    mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = o->type->protocol;
    mp_uint_t out_sz = stream_p->read(MP_OBJ_FROM_PTR(stream), buf, len, &native_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

off_t mp_stream_posix_lseek(void *stream, off_t offset, int whence) {
    const mp_obj_base_t* o = stream;
    const mp_stream_p_t *stream_p = o->type->protocol;
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
    const mp_stream_p_t *stream_p = o->type->protocol;
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_FLUSH, 0, &native_errno);
    if (res == MP_STREAM_ERROR) {
        return -1;
    }
    return res;
}

mp_obj_type_t btree_type;

#include "extmod/modbtree.c"

mp_map_elem_t btree_locals_dict_table[8];
STATIC MP_DEFINE_CONST_DICT(btree_locals_dict, btree_locals_dict_table);

STATIC mp_obj_t btree_open(size_t n_args, const mp_obj_t *args) {
    // Make sure we got a stream object
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);

    BTREEINFO openinfo = {0};
    openinfo.flags = mp_obj_get_int(args[1]);
    openinfo.cachesize = mp_obj_get_int(args[2]);
    openinfo.psize = mp_obj_get_int(args[3]);
    openinfo.minkeypage = mp_obj_get_int(args[4]);
    DB *db = __bt_open(MP_OBJ_TO_PTR(args[0]), &btree_stream_fvtable, &openinfo, 0);
    if (db == NULL) {
        mp_raise_OSError(native_errno);
    }

    return MP_OBJ_FROM_PTR(btree_new(db, args[0]));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(btree_open_obj, 5, 5, btree_open);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    btree_type.base.type = (void*)&mp_fun_table.type_type;
    btree_type.name = MP_QSTR_btree;
    btree_type.print = btree_print;
    btree_type.getiter = btree_getiter;
    btree_type.iternext = btree_iternext;
    btree_type.binary_op = btree_binary_op;
    btree_type.subscr = btree_subscr;
    btree_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_OBJ_FROM_PTR(&btree_close_obj) };
    btree_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_flush), MP_OBJ_FROM_PTR(&btree_flush_obj) };
    btree_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_get), MP_OBJ_FROM_PTR(&btree_get_obj) };
    btree_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_put), MP_OBJ_FROM_PTR(&btree_put_obj) };
    btree_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_seq), MP_OBJ_FROM_PTR(&btree_seq_obj) };
    btree_locals_dict_table[5] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_keys), MP_OBJ_FROM_PTR(&btree_keys_obj) };
    btree_locals_dict_table[6] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_values), MP_OBJ_FROM_PTR(&btree_values_obj) };
    btree_locals_dict_table[7] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_items), MP_OBJ_FROM_PTR(&btree_items_obj) };
    btree_type.locals_dict = (void*)&btree_locals_dict;

    mp_store_global(MP_QSTR__open, MP_OBJ_FROM_PTR(&btree_open_obj));
    mp_store_global(MP_QSTR_INCL, MP_OBJ_NEW_SMALL_INT(FLAG_END_KEY_INCL));
    mp_store_global(MP_QSTR_DESC, MP_OBJ_NEW_SMALL_INT(FLAG_DESC));

    MP_DYNRUNTIME_INIT_EXIT
}
