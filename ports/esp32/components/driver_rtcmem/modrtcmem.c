#include "py/obj.h"
#include "py/runtime.h"
#ifndef NO_QSTR
#include "driver_rtcmem.h"
#endif

#define STATIC static

// ---------------------------------------------------------------------------
// Integer API
// ---------------------------------------------------------------------------

STATIC mp_obj_t mod_rtcmem_int_read(mp_obj_t index_in) {
    int index = mp_obj_get_int(index_in);
    int value = 0;

    if (driver_rtcmem_int_read(index, &value) != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem int_read failed"));
    }

    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_rtcmem_int_read_obj, mod_rtcmem_int_read);

STATIC mp_obj_t mod_rtcmem_int_write(mp_obj_t index_in, mp_obj_t value_in) {
    int index = mp_obj_get_int(index_in);
    int value = mp_obj_get_int(value_in);

    if (driver_rtcmem_int_write(index, value) != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem int_write failed"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_rtcmem_int_write_obj, mod_rtcmem_int_write);

// We now clear both int + string via driver_rtcmem_clear()
STATIC mp_obj_t mod_rtcmem_int_clear(void) {
    if (driver_rtcmem_clear() != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem clear failed"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_rtcmem_int_clear_obj, mod_rtcmem_int_clear);

// ---------------------------------------------------------------------------
// String API
// ---------------------------------------------------------------------------

STATIC mp_obj_t mod_rtcmem_str_read(void) {
    const char *ptr = NULL;

    if (driver_rtcmem_string_read(&ptr) != ESP_OK || ptr == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem read_string failed"));
    }

    // Stored as a fixed-size buffer; treat as C string
    return mp_obj_new_str(ptr, strlen(ptr));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_rtcmem_str_read_obj, mod_rtcmem_str_read);

STATIC mp_obj_t mod_rtcmem_str_write(mp_obj_t str_in) {
    size_t len;
    const char *str = mp_obj_str_get_data(str_in, &len);

    // Truncation is handled inside driver_rtcmem_string_write
    if (driver_rtcmem_string_write(str) != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem write_string failed"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_rtcmem_str_write_obj, mod_rtcmem_str_write);

// For symmetry with the old API: clear string only
STATIC mp_obj_t mod_rtcmem_str_clear(void) {
    // We don't have a dedicated string-clear in the driver anymore,
    // so reuse driver_rtcmem_clear() and accept that it clears ints too.
    if (driver_rtcmem_clear() != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("rtcmem clear_string failed"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_rtcmem_str_clear_obj, mod_rtcmem_str_clear);

// ---------------------------------------------------------------------------
// Module globals
// ---------------------------------------------------------------------------

STATIC const mp_rom_map_elem_t rtcmem_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_rtcmem) },

    // Integer API
    { MP_ROM_QSTR(MP_QSTR_read),            MP_ROM_PTR(&mod_rtcmem_int_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mod_rtcmem_int_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear),           MP_ROM_PTR(&mod_rtcmem_int_clear_obj) },

    // String API
    { MP_ROM_QSTR(MP_QSTR_read_string),     MP_ROM_PTR(&mod_rtcmem_str_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_string),    MP_ROM_PTR(&mod_rtcmem_str_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_string),    MP_ROM_PTR(&mod_rtcmem_str_clear_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rtcmem_module_globals, rtcmem_module_globals_table);

const mp_obj_module_t mp_module_rtcmem = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rtcmem_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rtcmem, mp_module_rtcmem);
