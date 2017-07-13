#include "samd21_pins.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_D0), (mp_obj_t)&pin_PA08 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), (mp_obj_t)&pin_PA08 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1), (mp_obj_t)&pin_PA02 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), (mp_obj_t)&pin_PA02 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2), (mp_obj_t)&pin_PA09 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), (mp_obj_t)&pin_PA09 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D4), (mp_obj_t)&pin_PA06 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), (mp_obj_t)&pin_PA06 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D3), (mp_obj_t)&pin_PA07 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), (mp_obj_t)&pin_PA07 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13),(mp_obj_t)&pin_PA10 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_MOSI), (mp_obj_t)&pin_PA00 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_SCK), (mp_obj_t)&pin_PA01 },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
