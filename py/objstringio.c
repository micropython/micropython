#include <stdio.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "stream.h"

typedef struct _mp_obj_stringio_t {
    mp_obj_base_t base;
    vstr_t *vstr;
    // StringIO has single pointer used for both reading and writing
    machine_uint_t pos;
} mp_obj_stringio_t;

STATIC void stringio_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_stringio_t *self = self_in;
    print(env, "<io.StringIO 0x%x>", self->vstr);
}

STATIC machine_int_t stringio_read(mp_obj_t o_in, void *buf, machine_uint_t size, int *errcode) {
    mp_obj_stringio_t *o = o_in;
    machine_uint_t remaining = o->vstr->len - o->pos;
    if (size > remaining) {
        size = remaining;
    }
    memcpy(buf, o->vstr->buf + o->pos, size);
    o->pos += size;
    return size;
}

STATIC machine_int_t stringio_write(mp_obj_t o_in, const void *buf, machine_uint_t size, int *errcode) {
    mp_obj_stringio_t *o = o_in;
    machine_uint_t remaining = o->vstr->alloc - o->pos;
    if (size > remaining) {
        // Take all what's already allocated...
        o->vstr->len = o->vstr->alloc;
        // ... and add more
        vstr_add_len(o->vstr, size - remaining);
    }
    memcpy(o->vstr->buf + o->pos, buf, size);
    o->pos += size;
    if (o->pos > o->vstr->len) {
        o->vstr->len = o->pos;
    }
    return size;
}

STATIC mp_obj_t stringio_getvalue(mp_obj_t self_in) {
    mp_obj_stringio_t *self = self_in;
    return mp_obj_new_str((byte*)self->vstr->buf, self->vstr->len, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stringio_getvalue_obj, stringio_getvalue);

STATIC mp_obj_t stringio_close(mp_obj_t self_in) {
    mp_obj_stringio_t *self = self_in;
    vstr_free(self->vstr);
    self->vstr = NULL;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stringio_close_obj, stringio_close);

mp_obj_t stringio___exit__(uint n_args, const mp_obj_t *args) {
    return stringio_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stringio___exit___obj, 4, 4, stringio___exit__);

STATIC mp_obj_stringio_t *stringio_new() {
    mp_obj_stringio_t *o = m_new_obj(mp_obj_stringio_t);
    o->base.type = &mp_type_stringio;
    o->vstr = vstr_new();
    o->pos = 0;
    return o;
}

STATIC mp_obj_t stringio_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_stringio_t *o = stringio_new();

    if (n_args > 0) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
        stringio_write(o, bufinfo.buf, bufinfo.len, NULL);
        // Cur ptr is always at the beginning of buffer at the construction
        o->pos = 0;
    }
    return o;
}

STATIC const mp_map_elem_t stringio_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&stringio_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getvalue), (mp_obj_t)&stringio_getvalue_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___enter__), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___exit__), (mp_obj_t)&stringio___exit___obj },
};

STATIC MP_DEFINE_CONST_DICT(stringio_locals_dict, stringio_locals_dict_table);

STATIC const mp_stream_p_t stringio_stream_p = {
    .read = stringio_read,
    .write = stringio_write,
};

const mp_obj_type_t mp_type_stringio = {
    { &mp_type_type },
    .name = MP_QSTR_StringIO,
    .print = stringio_print,
    .make_new = stringio_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &stringio_stream_p,
    .locals_dict = (mp_obj_t)&stringio_locals_dict,
};
