#include "shared-bindings/board/__init__.h"

// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    // Left side header
    // Analog Pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_AREF),  MP_ROM_PTR(&pin_PA03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0),  MP_ROM_PTR(&pin_PA02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1),  MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2),  MP_ROM_PTR(&pin_PB00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3),  MP_ROM_PTR(&pin_PC00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4),  MP_ROM_PTR(&pin_PC01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A5),  MP_ROM_PTR(&pin_PC02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A6),  MP_ROM_PTR(&pin_PC03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DE1),  MP_ROM_PTR(&pin_PC03) },
    // Digital
    { MP_OBJ_NEW_QSTR(MP_QSTR_D0),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX2),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX2),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2),  MP_ROM_PTR(&pin_PC18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SERIAL_MODE2),  MP_ROM_PTR(&pin_PC18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D3),  MP_ROM_PTR(&pin_PC19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SERIAL_MODE1),  MP_ROM_PTR(&pin_PC19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D4),  MP_ROM_PTR(&pin_PC20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D5),  MP_ROM_PTR(&pin_PC10) },

    // Right Side Header
    { MP_OBJ_NEW_QSTR(MP_QSTR_D6),  MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DE2),  MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D7),  MP_ROM_PTR(&pin_PA13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D8),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D9),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D10),  MP_ROM_PTR(&pin_PD11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO),  MP_ROM_PTR(&pin_PD11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D11),  MP_ROM_PTR(&pin_PB20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D12),  MP_ROM_PTR(&pin_PB21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX1), MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PB12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX1), MP_ROM_PTR(&pin_PB12) },

    // Internal Devices
    // LED and Switch
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PB01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SWITCH), MP_ROM_PTR(&pin_PB02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PC24) },
    // Base Controller
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_MOSI), MP_ROM_PTR(&pin_PC22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_SCK), MP_ROM_PTR(&pin_PC23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_CS), MP_ROM_PTR(&pin_PD20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_MISO), MP_ROM_PTR(&pin_PD21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_READY), MP_ROM_PTR(&pin_PB16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BC_EN), MP_ROM_PTR(&pin_PB17) },
    // Internal I2C
    { MP_OBJ_NEW_QSTR(MP_QSTR_ATMAC_SDA), MP_ROM_PTR(&pin_PC16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ATMAC_SCL), MP_ROM_PTR(&pin_PC17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC_SDA), MP_ROM_PTR(&pin_PC16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC_SCL), MP_ROM_PTR(&pin_PC17) },
    // 24V Monitor
    { MP_OBJ_NEW_QSTR(MP_QSTR_P24V), MP_ROM_PTR(&pin_PC31) },
    // SD Card
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_PB26) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_PB27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_PB28) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_PB29) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CARD_DETECT), MP_ROM_PTR(&pin_PB31) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
