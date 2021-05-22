#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO0) },

    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO1) },

    { MP_ROM_QSTR(MP_QSTR_ESP_GPIO0), MP_ROM_PTR(&pin_GPIO2) },    // ESP32 FW boot state pin (confusingly named GPIO0)
    { MP_ROM_QSTR(MP_QSTR_ESP_RESET), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_ESP_BUSY), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_ESP_CS), MP_ROM_PTR(&pin_GPIO9) },

    // Primary SPI
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO7) },

    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_GPIO5) },

    // Secondary SPI connected to ESP32
    { MP_ROM_QSTR(MP_QSTR_MISO1), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_MOSI1), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_SCK1), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_CS1), MP_ROM_PTR(&pin_GPIO9) },

    // Primary I2C
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_GPIO12) }, // Note these are not actually analog!
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_GPIO13) }, // Note these are not actually analog!
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO13) },

    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_GPIO21) },

    // PDM onboard mic
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_DATA), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_CLOCK), MP_ROM_PTR(&pin_GPIO23) },

    // Sensor IRQ
    { MP_ROM_QSTR(MP_QSTR_INT1), MP_ROM_PTR(&pin_GPIO24) },

    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO29) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
