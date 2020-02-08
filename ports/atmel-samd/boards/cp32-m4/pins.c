#include "shared-bindings/board/__init__.h"

// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER_P),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPEAKER_N),  MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BATTERY),  MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STATUS_LED),  MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BACKLIGHT_PWM), MP_ROM_PTR(&pin_PB09) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SW1_2),  MP_ROM_PTR(&pin_PA06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW3_4),  MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW5), MP_ROM_PTR(&pin_PB12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW7), MP_ROM_PTR(&pin_PB06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW8), MP_ROM_PTR(&pin_PB03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW9), MP_ROM_PTR(&pin_PB05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SW10), MP_ROM_PTR(&pin_PB04) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_EXT_HDR3), MP_ROM_PTR(&pin_PB07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EXT_HDR4), MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EXT_HDR5),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EXT_SW6), MP_ROM_PTR(&pin_PA21) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD0),  MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD1),  MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD2), MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD3), MP_ROM_PTR(&pin_PB14) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_CMD), MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CLK), MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PB16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PB17) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_VSPI_SCK),  MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VSPI_MOSI),  MP_ROM_PTR(&pin_PB23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VSPI_MISO),  MP_ROM_PTR(&pin_PB22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VSPI_CS0),  MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VSPI_CS1),  MP_ROM_PTR(&pin_PA27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DC),  MP_ROM_PTR(&pin_PA23) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_SCK), MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102_MOSI), MP_ROM_PTR(&pin_PA12) },


    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
