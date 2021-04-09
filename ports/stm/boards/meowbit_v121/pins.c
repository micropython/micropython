#include "shared-bindings/board/__init__.h"
#include "supervisor/spi_flash_api.h"

#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-module/displayio/__init__.h"

extern audiopwmio_pwmaudioout_obj_t board_buzz_obj;

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_PTR(&pin_PB05) },

    { MP_ROM_QSTR(MP_QSTR_DISP_SCK), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_DISP_MISO), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_DISP_MOSI), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_DISP_CS), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_DISP_DC), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_DISP_RST), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_DISP_BL), MP_ROM_PTR(&pin_PB03) },

    { MP_ROM_QSTR(MP_QSTR_BUZZ), MP_ROM_PTR(&board_buzz_obj) },
    { MP_ROM_QSTR(MP_QSTR_BTNA), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_BTNB), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_TEMP), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_LIGHT), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_MENU), MP_ROM_PTR(&pin_PC15) },

    { MP_ROM_QSTR(MP_QSTR_JACK_TX), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_JACK_PWREN), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_JACK_SND), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_ACC_INT), MP_ROM_PTR(&pin_PC14) },
    { MP_ROM_QSTR(MP_QSTR_ACC_SCL), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_ACC_SDA), MP_ROM_PTR(&pin_PB07) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },
    // { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PC06) },  //these are wrong on Meowbit diagram.
    // { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA03) },  //they cannot be used together (UART2 vs 6)
    { MP_ROM_QSTR(MP_QSTR_NSS), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PC10) },

    { MP_ROM_QSTR(MP_QSTR_P20), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_P19), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_P16), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_P15), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_P14), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_P13), MP_ROM_PTR(&pin_PC10) },
    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_P12), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_P11), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_P10), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_P9), MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_P8), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_P1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_PA10) }, // in use by USB
    { MP_ROM_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_PC07) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_P0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_PB00) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},
    { MP_ROM_QSTR(MP_QSTR_INTERNAL_SPI), MP_ROM_PTR(&supervisor_flash_spi_bus) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
