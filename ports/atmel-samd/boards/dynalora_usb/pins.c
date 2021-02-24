#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PA30) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PA31) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA02) },

    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_CS), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_INT), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_RADIO_RESET), MP_ROM_PTR(&pin_PA10) },

    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_PA03) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA00) },

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA00) },

    { MP_ROM_QSTR(MP_QSTR_MOSI1), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_MISO1), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_SCK1), MP_ROM_PTR(&pin_PA01) },

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA19) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PA15) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
