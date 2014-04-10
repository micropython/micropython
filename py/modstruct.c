#include <assert.h>
#include <string.h>
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "objtuple.h"
#include "binary.h"

#if MICROPY_ENABLE_MOD_STRUCT

STATIC char get_fmt_type(const char **fmt) {
    char t = **fmt;
    switch (t) {
        case '!':
            t = '>';
            break;
        case '@':
        case '=':
        case '<':
        case '>':
            break;
        default:
            return '@';
    }
    // Skip type char
    (*fmt)++;
    return t;
}

STATIC uint calcsize_items(const char *fmt) {
    // TODO
    return strlen(fmt);
}

STATIC mp_obj_t struct_calcsize(mp_obj_t fmt_in) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);
    assert(fmt_type == '<' || fmt_type == '>'); (void)fmt_type;
    machine_uint_t size;
    for (size = 0; *fmt; fmt++) {
        int sz = mp_binary_get_size(*fmt);
        // TODO
        assert(sz != -1);
        size += sz;
    }
    return MP_OBJ_NEW_SMALL_INT(size);
}
MP_DEFINE_CONST_FUN_OBJ_1(struct_calcsize_obj, struct_calcsize);

STATIC mp_obj_t struct_unpack(mp_obj_t fmt_in, mp_obj_t data_in) {
    // TODO: "The buffer must contain exactly the amount of data required by the format (len(bytes) must equal calcsize(fmt))."
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);
    assert(fmt_type == '<' || fmt_type == '>'); (void)fmt_type;
    uint size = calcsize_items(fmt);
    mp_obj_tuple_t *res = mp_obj_new_tuple(size, NULL);
    buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo);
    byte *p = bufinfo.buf;

    for (uint i = 0; i < size; i++) {
        mp_obj_t item = mp_binary_get_val_unaligned(*fmt++, &p);
        res->items[i] = item;
    }
    return res;
}
MP_DEFINE_CONST_FUN_OBJ_2(struct_unpack_obj, struct_unpack);

STATIC const mp_map_elem_t mp_module_struct_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_struct) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_calcsize), (mp_obj_t)&struct_calcsize_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unpack), (mp_obj_t)&struct_unpack_obj },
};

STATIC const mp_obj_dict_t mp_module_struct_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = sizeof(mp_module_struct_globals_table) / sizeof(mp_map_elem_t),
        .alloc = sizeof(mp_module_struct_globals_table) / sizeof(mp_map_elem_t),
        .table = (mp_map_elem_t*)mp_module_struct_globals_table,
    },
};

const mp_obj_module_t mp_module_struct = {
    .base = { &mp_type_module },
    .name = MP_QSTR_struct,
    .globals = (mp_obj_dict_t*)&mp_module_struct_globals,
};

#endif
