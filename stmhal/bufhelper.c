#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "bufhelper.h"

void pyb_buf_get_for_send(mp_obj_t o, mp_buffer_info_t *bufinfo, byte *tmp_data) {
    if (MP_OBJ_IS_INT(o)) {
        tmp_data[0] = mp_obj_get_int(o);
        bufinfo->buf = tmp_data;
        bufinfo->len = 1;
        bufinfo->typecode = 'B';
    } else {
        mp_get_buffer_raise(o, bufinfo, MP_BUFFER_READ);
    }
}

mp_obj_t pyb_buf_get_for_recv(mp_obj_t o, mp_buffer_info_t *bufinfo) {
    if (MP_OBJ_IS_INT(o)) {
        // allocate a new bytearray of given length
        bufinfo->len = mp_obj_get_int(o);
        bufinfo->typecode = 'B';
        return mp_obj_str_builder_start(&mp_type_bytes, bufinfo->len, (byte**)&bufinfo->buf);
    } else {
        // get the existing buffer
        mp_get_buffer_raise(o, bufinfo, MP_BUFFER_WRITE);
        return MP_OBJ_NULL;
    }
}
