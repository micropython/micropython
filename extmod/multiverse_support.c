#include "extmod/multiverse_support.h"

#if __m68k
#define lm_base ((char *)0)
#else
char lm_base[65536];
#endif

MP_DECLARE_CTYPES_STRUCT(Point_obj);

#define DEFINE_PTR_SCALAR(type_c, tag) \
    const mp_rom_obj_tuple_t type_c##_ptr_tuple = ROM_TUPLE(MP_ROM_INT(UCTYPE_AGG(PTR)), MP_ROM_INT(UCTYPE_TYPE(tag))); \
    MP_DEFINE_CTYPES_STRUCT(type_c##_obj, MP_QSTR_##type_c, MP_ROM_PTR((void *)&type_c##_ptr_tuple), LAYOUT_NATIVE)

DEFINE_PTR_SCALAR(bool, UINT8);
DEFINE_PTR_SCALAR(char, UINT8);
DEFINE_PTR_SCALAR(uint8_t, UINT8);
DEFINE_PTR_SCALAR(uint16_t, UINT16);
DEFINE_PTR_SCALAR(uint32_t, UINT32);
DEFINE_PTR_SCALAR(uint64_t, UINT64);
DEFINE_PTR_SCALAR(int8_t, INT8);
DEFINE_PTR_SCALAR(int16_t, INT16);
DEFINE_PTR_SCALAR(int32_t, INT32);
DEFINE_PTR_SCALAR(int64_t, INT64);
DEFINE_PTR_SCALAR(float, FLOAT32);
DEFINE_PTR_SCALAR(double, FLOAT64);

static mp_uint_t get_agg_type(mp_obj_tuple_t *sub) {
    mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(sub->items[0]);
    mp_uint_t agg_type = GET_TYPE(offset, AGG_TYPE_BITS);
    return agg_type;
}

static mp_uint_t get_val_type(mp_obj_tuple_t *sub) {
    mp_int_t offset = MP_OBJ_SMALL_INT_VALUE(sub->items[1]);
    mp_uint_t val_type = GET_TYPE(offset, VAL_TYPE_BITS);
    return val_type;
}

static bool arg_compatible(const mp_obj_t arg_descr, const mp_obj_t type_descr) {
    // types are compatible if their descriptors are equal
    if (mp_obj_equal(arg_descr, type_descr)) {
        return true;
    }

    // Otherwise, they are compatible if
    //   * type is a pointer to T, and
    //   * arg is a pointer to T or array of T
    if (!mp_obj_is_type(type_descr, &mp_type_tuple)) {
        return false;
    }
    mp_obj_tuple_t *type_sub = MP_OBJ_TO_PTR(type_descr);
    if (get_agg_type(type_descr) != PTR) {
        return false;
    }

    if (!mp_obj_is_type(arg_descr, &mp_type_tuple)) {
        return false;
    }
    mp_obj_tuple_t *arg_sub = MP_OBJ_TO_PTR(arg_descr);

    if (arg_sub->len == 2) {
        // argument is a pointer-to-scalar or array-of-scalar, compare scalar types
        return get_val_type(type_sub) == get_val_type(arg_sub);
    } else {
        // argument is array-of-struct, compare descriptors
        return mp_obj_equal(type_sub->items[1], arg_sub->items[2]);
    }
}

static bool deref_compatible(const mp_obj_t arg_descr, const mp_obj_t type_descr) {
    if (!mp_obj_is_type(arg_descr, &mp_type_tuple)) {
        return false;
    }
    mp_obj_tuple_t *arg_sub = MP_OBJ_TO_PTR(arg_descr);
    if (get_agg_type(arg_sub) != PTR) {
        return false;
    }
    bool r = mp_obj_equal(type_descr, arg_sub->items[1]);
    return r;
}

#define mp_obj_get_type_qstr(o_in) ((qstr)(mp_obj_get_type((o_in))->name))
void *to_struct_helper(mp_obj_t obj, const mp_obj_type_t *struct_type, bool is_const, qstr fieldname) {
    if (obj == mp_const_none) {
        return NULL;
    }
    if (struct_type && !mp_obj_is_type(obj, struct_type)) {
        mp_obj_t arg_descr = uctypes_get_struct_desc(obj);
        mp_obj_t type_descr = uctypes_get_struct_desc(MP_OBJ_FROM_PTR(struct_type));

        if (deref_compatible(arg_descr, MP_OBJ_FROM_PTR(struct_type))) {
            // Consider the case of
            // ```py
            // w = WindowMgr.NewWindow(...)
            // qd.SetPort(w)
            // ```
            // which would otherwise say `TypeError: p must be of type GrafPort, not GrafPtr
            //
            // (currently) mkapi treats the argument to SetPort
            // as a GrafPort. But `GrafPtr` is a typedef to `GrafPort*`. It might be
            // better to fix this in mkapi, but instead cater here...

            mp_buffer_info_t bufinfo = {0};
            mp_get_buffer_raise(obj, &bufinfo, is_const ? MP_BUFFER_READ : MP_BUFFER_READ | MP_BUFFER_WRITE);
            void *result = *(void **)bufinfo.buf;
            return result;
        }
        if (!arg_compatible(arg_descr, type_descr)) {
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("%q must be of type %q, not %q"), fieldname, (qstr)struct_type->name, mp_obj_get_type_qstr(obj));
        }
    }
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(obj, &bufinfo, is_const ? MP_BUFFER_READ : MP_BUFFER_READ | MP_BUFFER_WRITE);
    return bufinfo.buf;
}

static const mp_rom_map_elem_t empty_table[] = {};
static MP_DEFINE_CONST_DICT(empty_dict, empty_table);

mp_obj_t from_struct_helper(void *buf, const mp_obj_type_t *type) {
    if (type) {
        mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)buf), MP_OBJ_FROM_PTR(type) };
        return uctypes_struct_make_new(type, 2, 0, args);
    } else {
        mp_obj_t args[] = { mp_obj_new_int((mp_uint_t)buf), MP_OBJ_FROM_PTR(&empty_dict) };
        return uctypes_struct_make_new(&uctypes_struct_type, 2, 0, args);
    }
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

void *void_ptr_from_py(mp_obj_t arg) {
    if (mp_obj_is_int(arg)) {
        return (void *)mp_obj_get_int_truncated(arg);
    }
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    return bufinfo.buf;
}

#if __m68k
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
#endif
