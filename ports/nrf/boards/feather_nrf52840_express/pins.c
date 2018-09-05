#include "shared-bindings/board/__init__.h"

#include "board_busses.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_P0_30) },
  { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_P0_28) },
  { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_P0_31) },
  { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_P0_02) },
  { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_P0_29) },
  { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_P0_03) },

  { MP_ROM_QSTR(MP_QSTR_AREF), MP_ROM_PTR(&pin_P0_04) },
  { MP_ROM_QSTR(MP_QSTR_VDIV), MP_ROM_PTR(&pin_P0_05) },

  { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_P1_13) },
  { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_P1_14) },
  { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_P1_15) },
  { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_P0_27) },
  { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_P0_26) },
  { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_P0_06) },
  { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_P0_07) },
  { MP_ROM_QSTR(MP_QSTR_L), MP_ROM_PTR(&pin_P0_07) },

  { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_P0_13) },

  { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_20) },
  { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_23) },
  { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_22) },

  { MP_ROM_QSTR(MP_QSTR_TXD), MP_ROM_PTR(&pin_P0_24) },
  { MP_ROM_QSTR(MP_QSTR_RXD), MP_ROM_PTR(&pin_P1_00) },

  { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P1_11) },
  { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P1_12) },

  { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_P1_02) },
  { MP_ROM_QSTR(MP_QSTR_LED_BLUE), MP_ROM_PTR(&pin_P1_10) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
