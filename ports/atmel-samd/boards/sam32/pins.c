#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_IO39), MP_ROM_PTR(&pin_PB08)  },
    { MP_ROM_QSTR(MP_QSTR_IO36), MP_ROM_PTR(&pin_PB09)  },
    { MP_ROM_QSTR(MP_QSTR_A6),   MP_ROM_PTR(&pin_PB04)  },
    { MP_ROM_QSTR(MP_QSTR_A7),   MP_ROM_PTR(&pin_PB05)  },
    { MP_ROM_QSTR(MP_QSTR_A8),   MP_ROM_PTR(&pin_PB06)  },
    { MP_ROM_QSTR(MP_QSTR_A9),   MP_ROM_PTR(&pin_PB07)  },

    { MP_ROM_QSTR(MP_QSTR_SCK),  MP_ROM_PTR(&pin_PB13)  },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB12)  },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB14)  },
    { MP_ROM_QSTR(MP_QSTR_xSDCS),MP_ROM_PTR(&pin_PA04)  },

    { MP_ROM_QSTR(MP_QSTR_DAC0), MP_ROM_PTR(&pin_PA02)  },
    { MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&pin_PA05)  },

    { MP_ROM_QSTR(MP_QSTR_D16),  MP_ROM_PTR(&pin_PA07)  },
    { MP_ROM_QSTR(MP_QSTR_D19),  MP_ROM_PTR(&pin_PA10)  },
    { MP_ROM_QSTR(MP_QSTR_D20),  MP_ROM_PTR(&pin_PA11)  },
    { MP_ROM_QSTR(MP_QSTR_D31),  MP_ROM_PTR(&pin_PA14)  },
    { MP_ROM_QSTR(MP_QSTR_D35),  MP_ROM_PTR(&pin_PA16)  },
    { MP_ROM_QSTR(MP_QSTR_D36),  MP_ROM_PTR(&pin_PA17)  },
    { MP_ROM_QSTR(MP_QSTR_D37),  MP_ROM_PTR(&pin_PA18)  },
    { MP_ROM_QSTR(MP_QSTR_D38),  MP_ROM_PTR(&pin_PA19)  },
    { MP_ROM_QSTR(MP_QSTR_D41),  MP_ROM_PTR(&pin_PA20)  },
    { MP_ROM_QSTR(MP_QSTR_D42),  MP_ROM_PTR(&pin_PA21)  },
    { MP_ROM_QSTR(MP_QSTR_D43),  MP_ROM_PTR(&pin_PA22)  },
    { MP_ROM_QSTR(MP_QSTR_D44),  MP_ROM_PTR(&pin_PA23)  },
    { MP_ROM_QSTR(MP_QSTR_D49),  MP_ROM_PTR(&pin_PB22)  },
    { MP_ROM_QSTR(MP_QSTR_D50),  MP_ROM_PTR(&pin_PB23)  },
    { MP_ROM_QSTR(MP_QSTR_D59),  MP_ROM_PTR(&pin_PB30)  },
    { MP_ROM_QSTR(MP_QSTR_D60),  MP_ROM_PTR(&pin_PB31)  },
    { MP_ROM_QSTR(MP_QSTR_D61),  MP_ROM_PTR(&pin_PB00)  },

    { MP_ROM_QSTR(MP_QSTR_IO13),  MP_ROM_PTR(&pin_PB01)  },
    { MP_ROM_QSTR(MP_QSTR_IO12),  MP_ROM_PTR(&pin_PB02)  },
    { MP_ROM_QSTR(MP_QSTR_IO14),  MP_ROM_PTR(&pin_PB03)  },

    { MP_ROM_QSTR(MP_QSTR_TCK),   MP_ROM_PTR(&pin_PB01)  },
    { MP_ROM_QSTR(MP_QSTR_TDI),   MP_ROM_PTR(&pin_PB02)  },
    { MP_ROM_QSTR(MP_QSTR_TMS),   MP_ROM_PTR(&pin_PB03)  },
    { MP_ROM_QSTR(MP_QSTR_ESP_CS),MP_ROM_PTR(&pin_PB15)  },

    { MP_ROM_QSTR(MP_QSTR_TX1),   MP_ROM_PTR(&pin_PA12)  },
    { MP_ROM_QSTR(MP_QSTR_RX1),   MP_ROM_PTR(&pin_PA13)  },
    { MP_ROM_QSTR(MP_QSTR_TX2),   MP_ROM_PTR(&pin_PB16)  },
    { MP_ROM_QSTR(MP_QSTR_RX2),   MP_ROM_PTR(&pin_PB17)  },

    { MP_ROM_QSTR(MP_QSTR_SDA),  MP_ROM_PTR(&pin_PA08)  },
    { MP_ROM_QSTR(MP_QSTR_SCL),  MP_ROM_PTR(&pin_PA09)  },

    { MP_ROM_QSTR(MP_QSTR_RTS),  MP_ROM_PTR(&pin_PB11)  },
    { MP_ROM_QSTR(MP_QSTR_DTR),  MP_ROM_PTR(&pin_PB10)  },

    { MP_ROM_QSTR(MP_QSTR_LED),       MP_ROM_PTR(&pin_PA27)  },
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL),  MP_ROM_PTR(&pin_PA15)  },

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C),  MP_ROM_PTR(&board_i2c_obj)  },
    { MP_ROM_QSTR(MP_QSTR_SPI),  MP_ROM_PTR(&board_spi_obj)  },
    { MP_ROM_QSTR(MP_QSTR_BATTERY),   MP_ROM_PTR(&pin_PA06) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
