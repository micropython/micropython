#include "samd21_pins.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_A0), (mp_obj_t)&pin_PA02 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D0), (mp_obj_t)&pin_PA11 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D1), (mp_obj_t)&pin_PA10 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D3), (mp_obj_t)&pin_PA09 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D4), (mp_obj_t)&pin_PA08 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_L),  (mp_obj_t)&pin_PA28 },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
