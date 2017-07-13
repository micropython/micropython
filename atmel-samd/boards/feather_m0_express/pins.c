#include "samd21_pins.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), (mp_obj_t)&pin_PA02 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1), (mp_obj_t)&pin_PB08 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2), (mp_obj_t)&pin_PB09 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3), (mp_obj_t)&pin_PA04 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4), (mp_obj_t)&pin_PA05 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A5), (mp_obj_t)&pin_PB02 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK), (mp_obj_t)&pin_PB11 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), (mp_obj_t)&pin_PB10 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), (mp_obj_t)&pin_PA12 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D0), (mp_obj_t)&pin_PA11 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), (mp_obj_t)&pin_PA11 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1), (mp_obj_t)&pin_PA10 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), (mp_obj_t)&pin_PA10 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), (mp_obj_t)&pin_PA22 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), (mp_obj_t)&pin_PA23 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D5), (mp_obj_t)&pin_PA15 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D6), (mp_obj_t)&pin_PA20 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D9), (mp_obj_t)&pin_PA07 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D10), (mp_obj_t)&pin_PA18 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D11), (mp_obj_t)&pin_PA16 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D12), (mp_obj_t)&pin_PA19 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13), (mp_obj_t)&pin_PA17 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), (mp_obj_t)&pin_PA06 },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
