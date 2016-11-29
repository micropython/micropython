#include "samd21_pins.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_D10), (mp_obj_t)&pin_PA06 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D9), (mp_obj_t)&pin_PA07 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D6), (mp_obj_t)&pin_PA04 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D12), (mp_obj_t)&pin_PA05 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_D0), (mp_obj_t)&pin_PA11 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_RX), (mp_obj_t)&pin_PA11 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D1), (mp_obj_t)&pin_PA10 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TX), (mp_obj_t)&pin_PA10 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), (mp_obj_t)&pin_PB22 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_MICROPHONE), (mp_obj_t)&pin_PA08 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT), (mp_obj_t)&pin_PA14 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_ACCELEROMETER_CS),   (mp_obj_t)&pin_PA13 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_LEFT_BUTTON), (mp_obj_t)&pin_PA28 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT_BUTTON), (mp_obj_t)&pin_PA19 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_SLIDE_SWITCH), (mp_obj_t)&pin_PB09 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER), (mp_obj_t)&pin_PA02 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_LIGHT), (mp_obj_t)&pin_PB02 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TEMPERATURE), (mp_obj_t)&pin_PA09 },

  { MP_OBJ_NEW_QSTR(MP_QSTR_D13), (mp_obj_t)&pin_PA17 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), (mp_obj_t)&pin_PA22 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D2), (mp_obj_t)&pin_PA22 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), (mp_obj_t)&pin_PA23 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_D3), (mp_obj_t)&pin_PA23 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SCK),  (mp_obj_t)&pin_PB11 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), (mp_obj_t)&pin_PB10 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), (mp_obj_t)&pin_PA12 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_FLASH_CS),   (mp_obj_t)&pin_PB08 },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
