#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "stream.h"

// This file defines generic Python stream read/write methods which
// dispatch to the underlying stream interface of an object.

static mp_obj_t stream_read(mp_obj_t self_in, mp_obj_t arg) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p.read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_OSError, "Operation not supported"));
    }

    machine_int_t sz = mp_obj_get_int(arg);
    // +1 because so far we mark end of string with \0
    char *buf = m_new(char, sz + 1);
    int error;
    machine_int_t out_sz = o->type->stream_p.read(self_in, buf, sz, &error);
    if (out_sz == -1) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_OSError, "[Errno %d]", (const char *)error));
    } else {
        buf[out_sz] = 0;
        return mp_obj_new_str(qstr_from_str_take(buf, /*out_sz,*/ sz + 1));
    }
}

static mp_obj_t stream_write(mp_obj_t self_in, mp_obj_t arg) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p.write == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_OSError, "Operation not supported"));
    }

    const char *buf = qstr_str(mp_obj_get_qstr(arg));
    machine_int_t sz = strlen(buf);
    int error;
    machine_int_t out_sz = o->type->stream_p.write(self_in, buf, sz, &error);
    if (out_sz == -1) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_OSError, "[Errno %d]", (const char *)error));
    } else {
        // http://docs.python.org/3/library/io.html#io.RawIOBase.write
        // "None is returned if the raw stream is set not to block and no single byte could be readily written to it."
        // Do they mean that instead of 0 they return None?
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_read_obj, stream_read);
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write_obj, stream_write);
