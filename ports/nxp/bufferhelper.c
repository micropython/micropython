#include <stdio.h>

#include "py/obj.h"
#include "py/runtime.h"

void pyb_buf_get_for_send(mp_obj_t o, mp_buffer_info_t *bufinfo, byte *tmp_data)
{
    if (mp_obj_is_int(o)) {
        tmp_data[0] = mp_obj_get_int(o);
        bufinfo->buf = tmp_data;
        bufinfo->len = 1;
        bufinfo->typecode = 'B';
    } else {
        mp_get_buffer_raise(o, bufinfo, MP_BUFFER_READ);
    }
}

mp_obj_t pyb_buf_get_for_recv(mp_obj_t o, vstr_t *vstr)
{
    if (mp_obj_is_int(o)) {
        // allocate a new bytearray of given length
        vstr_init_len(vstr, mp_obj_get_int(o));
        return MP_OBJ_NULL;
    } else {
        // get the existing buffer
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(o, &bufinfo, MP_BUFFER_WRITE);
        vstr->buf = bufinfo.buf;
        vstr->len = bufinfo.len;
        return o;
    }
}
