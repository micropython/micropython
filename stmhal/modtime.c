#include <stm32f4xx_hal.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "modtime.h"

STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
#if MICROPY_ENABLE_FLOAT
    if (MP_OBJ_IS_INT(seconds_o)) {
#endif
        HAL_Delay(1000 * mp_obj_get_int(seconds_o));
#if MICROPY_ENABLE_FLOAT
    } else {
        HAL_Delay((uint32_t)(1000 * mp_obj_get_float(seconds_o)));
    }
#endif
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_time) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&time_sleep_obj },
};

STATIC const mp_map_t time_module_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(time_module_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(time_module_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)time_module_globals_table,
};

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_time,
    .globals = (mp_map_t*)&time_module_globals,
};
