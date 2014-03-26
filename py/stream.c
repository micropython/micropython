#include <unistd.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "stream.h"

// This file defines generic Python stream read/write methods which
// dispatch to the underlying stream interface of an object.

STATIC mp_obj_t stream_readall(mp_obj_t self_in);

STATIC mp_obj_t stream_read(uint n_args, const mp_obj_t *args) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)args[0];
    if (o->type->stream_p.read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    machine_int_t sz;
    if (n_args == 1 || ((sz = mp_obj_get_int(args[1])) == -1)) {
        return stream_readall(args[0]);
    }
    byte *buf = m_new(byte, sz);
    int error;
    machine_int_t out_sz = o->type->stream_p.read(o, buf, sz, &error);
    if (out_sz == -1) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
    } else {
        mp_obj_t s = mp_obj_new_str(buf, out_sz, false); // will reallocate to use exact size
        m_free(buf, sz);
        return s;
    }
}

STATIC mp_obj_t stream_write(mp_obj_t self_in, mp_obj_t arg) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p.write == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    uint sz;
    const char *buf = mp_obj_str_get_data(arg, &sz);
    int error;
    machine_int_t out_sz = o->type->stream_p.write(self_in, buf, sz, &error);
    if (out_sz == -1) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
    } else {
        // http://docs.python.org/3/library/io.html#io.RawIOBase.write
        // "None is returned if the raw stream is set not to block and no single byte could be readily written to it."
        // Do they mean that instead of 0 they return None?
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

// TODO: should be in mpconfig.h
#define READ_SIZE 256
STATIC mp_obj_t stream_readall(mp_obj_t self_in) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p.read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    int total_size = 0;
    vstr_t *vstr = vstr_new_size(READ_SIZE);
    char *buf = vstr_str(vstr);
    char *p = buf;
    int error;
    int current_read = READ_SIZE;
    while (true) {
        machine_int_t out_sz = o->type->stream_p.read(self_in, p, current_read, &error);
        if (out_sz == -1) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
        }
        if (out_sz == 0) {
            break;
        }
        total_size += out_sz;
        if (out_sz < current_read) {
            current_read -= out_sz;
            p += out_sz;
        } else {
            current_read = READ_SIZE;
            p = vstr_extend(vstr, current_read);
            if (p == NULL) {
                // TODO
                nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError/*&mp_type_RuntimeError*/, "Out of memory"));
            }
        }
    }

    mp_obj_t s = mp_obj_new_str((byte*)vstr->buf, total_size, false);
    vstr_free(vstr);
    return s;
}

// Unbuffered, inefficient implementation of readline() for raw I/O files.
STATIC mp_obj_t stream_unbuffered_readline(uint n_args, const mp_obj_t *args) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)args[0];
    if (o->type->stream_p.read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    machine_int_t max_size = -1;
    if (n_args > 1) {
        max_size = MP_OBJ_SMALL_INT_VALUE(args[1]);
    }

    vstr_t *vstr;
    if (max_size != -1) {
        vstr = vstr_new_size(max_size);
    } else {
        vstr = vstr_new();
    }

    int error;
    while (max_size == -1 || max_size-- != 0) {
        char *p = vstr_add_len(vstr, 1);
        if (p == NULL) {
            // TODO
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError/*&mp_type_RuntimeError*/, "Out of memory"));
        }

        machine_int_t out_sz = o->type->stream_p.read(o, p, 1, &error);
        if (out_sz == -1) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
        }
        if (out_sz == 0) {
            // Back out previously added byte
            // TODO: This is a bit hacky, does it supported by vstr API contract?
            // Consider, what's better - read a char and get OutOfMemory (so read
            // char is lost), or allocate first as we do.
            vstr_add_len(vstr, -1);
            break;
        }
        if (*p == '\n') {
            break;
        }
    }
    // TODO don't intern this string
    vstr_shrink(vstr);
    return MP_OBJ_NEW_QSTR(qstr_from_strn_take(vstr_str(vstr), vstr->alloc, vstr_len(vstr)));
}

mp_obj_t mp_stream_unbuffered_iter(mp_obj_t self) {
    mp_obj_t l_in = stream_unbuffered_readline(1, &self);
    uint sz;
    mp_obj_str_get_data(l_in, &sz);
    if (sz != 0) {
        return l_in;
    }
    return MP_OBJ_NULL;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read_obj, 1, 2, stream_read);
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_readall_obj, stream_readall);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_unbuffered_readline_obj, 1, 2, stream_unbuffered_readline);
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write_obj, stream_write);
