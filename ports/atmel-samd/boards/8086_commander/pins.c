#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {

    // Serial
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA11) }, // RX
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA10) }, // TX
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA10) },

    // Buttons
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA20) }, // 1
    { MP_ROM_QSTR(MP_QSTR_B1), MP_ROM_PTR(&pin_PA20) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PA09) }, // 2
    { MP_ROM_QSTR(MP_QSTR_B2), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PB09) }, // 3
    { MP_ROM_QSTR(MP_QSTR_B3), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PA02) }, // 4
    { MP_ROM_QSTR(MP_QSTR_B4), MP_ROM_PTR(&pin_PA02) },

    // SD Card / SPI
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PA13) }, // CS
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PA13) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PB10) }, // MOSI
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PB11) }, // SCK
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PA12) }, // MISO
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA12) },

    // LED
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA15) }, // 1A
    { MP_ROM_QSTR(MP_QSTR_LED1A), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA14) }, // 1B
    { MP_ROM_QSTR(MP_QSTR_LED1B), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA08) }, // 2A
    { MP_ROM_QSTR(MP_QSTR_LED2A), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA07) }, // 2B
    { MP_ROM_QSTR(MP_QSTR_LED2B), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PA06) }, // ALERT
    { MP_ROM_QSTR(MP_QSTR_ALERT), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_PA05) }, // 3A
    { MP_ROM_QSTR(MP_QSTR_LED3A), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_PA04) }, // 3B
    { MP_ROM_QSTR(MP_QSTR_LED3B), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_PB02) }, // 4A
    { MP_ROM_QSTR(MP_QSTR_LED4A), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_PB03) }, // 4B
    { MP_ROM_QSTR(MP_QSTR_LED4B), MP_ROM_PTR(&pin_PB03) },

    // Accelerometer / I2C
    { MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_PA23) }, // SCL
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA23) },
    { MP_ROM_QSTR(MP_QSTR_D20), MP_ROM_PTR(&pin_PA22) }, // SDA
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA22) },

    // Comm objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
