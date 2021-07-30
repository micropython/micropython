#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {

    // Analog pins
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA06) },

    // Digital pins
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PA06) },

    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PA17) }, // MP_QSTR_SDA
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA16) }, // MP_QSTR_SCL
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA15) }, // MP_QSTR_LED

    // UART pins
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PB26) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PB27) },

    // SPI pins
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PB01) },

    // I2C pins
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA17) },

    // LED pins
    { MP_ROM_QSTR(MP_QSTR_LED),   MP_ROM_PTR(&pin_PA15) }, // status

    // LCD Display
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_MISO),  MP_ROM_PTR(&pin_PB18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_MOSI),  MP_ROM_PTR(&pin_PB19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_SCK),  MP_ROM_PTR(&pin_PB20) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS),  MP_ROM_PTR(&pin_PB21) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC),  MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT),  MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RESET),  MP_ROM_PTR(&pin_PC07) },

    // SD Card
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MISO),  MP_ROM_PTR(&pin_PC18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MOSI),  MP_ROM_PTR(&pin_PC16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_SCK),  MP_ROM_PTR(&pin_PC17) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS),  MP_ROM_PTR(&pin_PC19) },
    { MP_ROM_QSTR(MP_QSTR_SD_DET),  MP_ROM_PTR(&pin_PD21) },

    // Switch
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH_UP),  MP_ROM_PTR(&pin_PD20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH_LEFT),  MP_ROM_PTR(&pin_PD12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH_RIGHT),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH_DOWN),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH_PRESS),  MP_ROM_PTR(&pin_PD10) },

    // Button
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_1),  MP_ROM_PTR(&pin_PC26) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_2),  MP_ROM_PTR(&pin_PC27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_3),  MP_ROM_PTR(&pin_PC28) },

    // Special named pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_LIGHT),  MP_ROM_PTR(&pin_PD01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUZZER),  MP_ROM_PTR(&pin_PD11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IR),  MP_ROM_PTR(&pin_PB31) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MIC),  MP_ROM_PTR(&pin_PC30) },
    { MP_ROM_QSTR(MP_QSTR_GYROSCOPE_SCL),  MP_ROM_PTR(&pin_PA12) },
    { MP_ROM_QSTR(MP_QSTR_GYROSCOPE_SDA),  MP_ROM_PTR(&pin_PA13) },
    { MP_ROM_QSTR(MP_QSTR_GYROSCOPE_INT),  MP_ROM_PTR(&pin_PC21) },

    // DAC
    { MP_OBJ_NEW_QSTR(MP_QSTR_DAC0),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DAC1),  MP_ROM_PTR(&pin_PA05) },

    // I2S
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2S_LRCLK),  MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2S_SDOUT),  MP_ROM_PTR(&pin_PA21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2S_SDIN),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2S_BCLK),  MP_ROM_PTR(&pin_PB16) },

    // RTL8720D
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_PWR),  MP_ROM_PTR(&pin_PA18) },       // CHIP_PU
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_RXD),  MP_ROM_PTR(&pin_PC22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_TXD),  MP_ROM_PTR(&pin_PC23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_MOSI),  MP_ROM_PTR(&pin_PB24) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_CLK),  MP_ROM_PTR(&pin_PB25) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_MISO),  MP_ROM_PTR(&pin_PC24) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_CS),  MP_ROM_PTR(&pin_PC25) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_READY),  MP_ROM_PTR(&pin_PC20) },     // IRQ0
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTL_DIR),  MP_ROM_PTR(&pin_PA19) },       // SYNC

    // Comm objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
