#include "shared-bindings/board/__init__.h"

#include "supervisor/board.h"
#include "shared-module/displayio/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0),  MP_ROM_PTR(&pin_PA02) }, // A0 = audio right channel
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1),  MP_ROM_PTR(&pin_PA05) }, // A1 = audio left channel
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2),  MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3),  MP_ROM_PTR(&pin_PB09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4),  MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A5),  MP_ROM_PTR(&pin_PA06) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_A6),  MP_ROM_PTR(&pin_PB01) }, // A6 = VBAT Monitor
    { MP_OBJ_NEW_QSTR(MP_QSTR_A7),  MP_ROM_PTR(&pin_PB04) }, // A7 = Raw mic input
    { MP_OBJ_NEW_QSTR(MP_QSTR_A8),  MP_ROM_PTR(&pin_PB03) }, // A8 = Bottom STEMMA Port
    { MP_OBJ_NEW_QSTR(MP_QSTR_A9),  MP_ROM_PTR(&pin_PB02) }, // A9 = Top STEMMA Port
    { MP_OBJ_NEW_QSTR(MP_QSTR_A10),  MP_ROM_PTR(&pin_PA07) }, // A10 = Amplified mic input
    { MP_OBJ_NEW_QSTR(MP_QSTR_A11),  MP_ROM_PTR(&pin_PB00) }, // A11 = VBUS Monitor

    { MP_OBJ_NEW_QSTR(MP_QSTR_D0),  MP_ROM_PTR(&pin_PB17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1),  MP_ROM_PTR(&pin_PB16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2),  MP_ROM_PTR(&pin_PB03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D3),  MP_ROM_PTR(&pin_PB02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D5),  MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D6),  MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D9),  MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D10),  MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D11),  MP_ROM_PTR(&pin_PA21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D12),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D48),  MP_ROM_PTR(&pin_PB31) },

    // UART
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX),  MP_ROM_PTR(&pin_PB17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX),  MP_ROM_PTR(&pin_PB16) },

    // I2C
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),  MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),  MP_ROM_PTR(&pin_PA13) },

    // SPI
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK),  MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO),  MP_ROM_PTR(&pin_PB22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI),  MP_ROM_PTR(&pin_PB23) },

    // e-paper SPI and control pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK1),  MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI1),  MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ECS),  MP_ROM_PTR(&pin_PB07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EDC),  MP_ROM_PTR(&pin_PB05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ERST),  MP_ROM_PTR(&pin_PA00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EBSY),  MP_ROM_PTR(&pin_PA01) },

    // Special named pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_BATTERY),  MP_ROM_PTR(&pin_PB01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USB),  MP_ROM_PTR(&pin_PB00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MICIN),  MP_ROM_PTR(&pin_PB04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MICOUT),  MP_ROM_PTR(&pin_PA07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL),  MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LOCK_BUTTON),  MP_ROM_PTR(&pin_PA27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_LATCH),  MP_ROM_PTR(&pin_PB12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_OUT),  MP_ROM_PTR(&pin_PB30) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BUTTON_CLOCK),  MP_ROM_PTR(&pin_PB14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDCS),  MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MIC_SHUTDOWN),  MP_ROM_PTR(&pin_PB31) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BCS),  MP_ROM_PTR(&pin_PB06) }, // BCS = Babel Chip Select, the second flash chip

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].epaper_display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
