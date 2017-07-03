#include "samd21_pins.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1),  (mp_obj_t)&pin_PA05 }, // pad 1
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2),  (mp_obj_t)&pin_PA05 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX),  (mp_obj_t)&pin_PA05 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), (mp_obj_t)&pin_PA05 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_A2),  (mp_obj_t)&pin_PA04 }, // pad 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_D0),  (mp_obj_t)&pin_PA04 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX),  (mp_obj_t)&pin_PA04 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), (mp_obj_t)&pin_PA04 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), (mp_obj_t)&pin_PA02 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1), (mp_obj_t)&pin_PA02 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_L), (mp_obj_t)&pin_PA23 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13), (mp_obj_t)&pin_PA23 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_MOSI), (mp_obj_t)&pin_PA00 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_SCK), (mp_obj_t)&pin_PA01 },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
