#include "shared-bindings/board/__init__.h"
#include "supervisor/board.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SCK),     MP_ROM_PTR(&pin_PA05)  },
    { MP_ROM_QSTR(MP_QSTR_MOSI),    MP_ROM_PTR(&pin_PA07)  },
    { MP_ROM_QSTR(MP_QSTR_MISO),    MP_ROM_PTR(&pin_PA04)  },

    { MP_ROM_QSTR(MP_QSTR_radioCS), MP_ROM_PTR(&pin_PA00)  },
    { MP_ROM_QSTR(MP_QSTR_WAKE),    MP_ROM_PTR(&pin_PA01)  },
    { MP_ROM_QSTR(MP_QSTR_SHDWN),   MP_ROM_PTR(&pin_PB10)  },
    { MP_ROM_QSTR(MP_QSTR_PWDWN),   MP_ROM_PTR(&pin_PB11)  },
    { MP_ROM_QSTR(MP_QSTR_TST),     MP_ROM_PTR(&pin_PA11)  },
    { MP_ROM_QSTR(MP_QSTR_FSYNC),   MP_ROM_PTR(&pin_PA13)  },
    { MP_ROM_QSTR(MP_QSTR_VCLK),    MP_ROM_PTR(&pin_PA14)  },
    { MP_ROM_QSTR(MP_QSTR_FSYNC),   MP_ROM_PTR(&pin_PA15)  },
    { MP_ROM_QSTR(MP_QSTR_MD),          MP_ROM_PTR(&pin_PA18)  },
    { MP_ROM_QSTR(MP_QSTR_MC),          MP_ROM_PTR(&pin_PA19)  },

    { MP_ROM_QSTR(MP_QSTR_PA08),        MP_ROM_PTR(&pin_PA08)  },
    { MP_ROM_QSTR(MP_QSTR_PA10),        MP_ROM_PTR(&pin_PA10)  },
    { MP_ROM_QSTR(MP_QSTR_PA09),        MP_ROM_PTR(&pin_PA09)  },
    { MP_ROM_QSTR(MP_QSTR_PA06),        MP_ROM_PTR(&pin_PA06)  },
    { MP_ROM_QSTR(MP_QSTR_DAC0),        MP_ROM_PTR(&pin_PA02)  },

    { MP_ROM_QSTR(MP_QSTR_TX),      MP_ROM_PTR(&pin_PB09)  },
    { MP_ROM_QSTR(MP_QSTR_RX),      MP_ROM_PTR(&pin_PB08)  },
    { MP_ROM_QSTR(MP_QSTR_SDA),     MP_ROM_PTR(&pin_PA16)  },
    { MP_ROM_QSTR(MP_QSTR_SCL),     MP_ROM_PTR(&pin_PA17)  },


    { MP_ROM_QSTR(MP_QSTR_LED),     MP_ROM_PTR(&pin_PB03)  },

    { MP_ROM_QSTR(MP_QSTR_UART),    MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C),     MP_ROM_PTR(&board_i2c_obj)  },
    { MP_ROM_QSTR(MP_QSTR_SPI),     MP_ROM_PTR(&board_spi_obj)  },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
