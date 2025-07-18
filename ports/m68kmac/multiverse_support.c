#include "multiverse_support.h"

MP_DECLARE_CTYPES_STRUCT(Point_obj);

#define mp_obj_get_type_qstr(o_in) ((qstr)(mp_obj_get_type((o_in))->name))
void *to_struct_helper(mp_obj_t obj, const mp_obj_type_t *struct_type, bool is_const, qstr fieldname) {
    if (obj == mp_const_none) {
        return NULL;
    }
    if (struct_type && !mp_obj_is_type(obj, struct_type)) {
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("%q must be of type %q, not %q"), fieldname, (qstr)struct_type->name, mp_obj_get_type_qstr(obj));
    }
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(obj, &bufinfo, is_const ? MP_BUFFER_READ : MP_BUFFER_READ | MP_BUFFER_WRITE);
    return bufinfo.buf;
}


mp_obj_t from_struct_helper(void *buf, const mp_obj_type_t *type) {
    mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)buf), MP_OBJ_FROM_PTR(type) };
    return uctypes_struct_make_new(type, 2, 0, args);
}

void *to_scalar_helper(mp_obj_t obj, size_t objsize, bool is_const) {
    if (mp_obj_is_int(obj)) {
        return (void *)mp_obj_get_int_truncated(obj);
    }
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(obj, &bufinfo, is_const ? MP_BUFFER_READ : MP_BUFFER_READ | MP_BUFFER_WRITE);
    if (objsize > 1 && bufinfo.len != objsize) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
    }
    return bufinfo.buf;
}


mp_obj_t from_scalar_helper(void *buf, size_t objsize, bool is_signed_hint) {
    return mp_obj_new_int_from_uint(*(unsigned long *)buf);
}

mp_obj_t LMGet_common(long address, size_t objsize, mp_obj_t arg) {
    if (arg == mp_const_none) {
        return mp_obj_new_bytearray(objsize, (void *)address);
    }
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len != objsize) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
    }
    memcpy(bufinfo.buf, (void *)address, objsize);
    return arg;
}
void LMSet_common(long address, size_t objsize, mp_obj_t arg) {
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len != objsize) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
    }
    memcpy((void *)address, bufinfo.buf, objsize);
}

Point Point_to_c(mp_obj_t obj, qstr fieldname) {
    Point result;
    if (mp_obj_len_maybe(obj) == MP_OBJ_NEW_SMALL_INT(2)) {
        result.h = mp_obj_get_int(mp_obj_subscr(obj, mp_obj_new_int(0), MP_OBJ_SENTINEL));
        result.v = mp_obj_get_int(mp_obj_subscr(obj, mp_obj_new_int(1), MP_OBJ_SENTINEL));
    } else {
        result = *(Point *)to_struct_helper(obj, (const mp_obj_type_t *)&Point_obj, true, fieldname);
    }
    return result;
}

pascal LONGINT GetDblTime(void) {
    return LMGetDoubleTime();
}

pascal LONGINT GetCaretTime(void) {
    return LMGetCaretTime();
}

typedef struct {
    INTEGER count;
    Byte data[0];
} patternlist;

pascal void GetIndPattern(Byte *pat, INTEGER patListId, INTEGER index) {
    Handle h = (Handle)GetResource('PAT#', patListId);
    if (!h) {
        return;
    }
    LoadResource(h);
    if (ResError() != noErr) {
        return;
    }
    patternlist *patterns = (patternlist *)*h;
    if (index < 0 || index >= patterns->count) {
        return;
    }
    memcpy(pat, patterns->data + 8 * index, 8);
}
