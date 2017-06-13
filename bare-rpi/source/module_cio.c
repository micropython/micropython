#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "pios/uart.h"


STATIC mp_obj_t cio_write( mp_obj_t str ) {
    size_t len;
    const char* ptr = mp_obj_str_get_str  ( str);
    pios_uart_write ( ptr, strlen(ptr) );
    return mp_const_none;
}
STATIC mp_obj_t cio_read () {
    return mp_const_none;
}

STATIC mp_obj_t cio_getchar () {
    return mp_obj_new_int_from_uint ( pios_uart_getchar() );
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(cio_write_obj, cio_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cio_getchar_obj, cio_getchar);

STATIC const mp_map_elem_t cio_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cio) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&cio_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getchar), (mp_obj_t)&cio_getchar_obj }
};

STATIC MP_DEFINE_CONST_DICT(mp_cio_globals, cio_globals_table);

const mp_obj_module_t mp_module_cio = {
    .base = { &mp_type_module },
   // .name = MP_QSTR_cbyte,
    .globals = (mp_obj_dict_t*)&mp_cio_globals,
};

